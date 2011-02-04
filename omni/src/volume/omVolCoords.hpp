#ifndef OM_VOL_COORDS_HPP
#define OM_VOL_COORDS_HPP

/*
 * OmVolume is the abstract represntation of multiple forms of data enclosed by
 *   a region of space.
 *
 *  Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "common/omCommon.h"

class OmVolCoords {
protected:
    //transforms from normailzed unit cube external spatial coverage
    Matrix4f spaceToUserMat_;
    Matrix4f spaceToUserInvMat_;

    Matrix4f normToDataMat_;
    Matrix4f normToDataInvMat_;

    //data properties
    DataBbox dataExtent_;
    Vector3f dataResolution_;  //units per voxel
    Vector3f dataStretchValues_;

    int chunkDim_;
    QString unitString_;

public:
//transform methods
    inline Vector3f GetScale() const
    {
        return Vector3f(normToDataMat_.m00,
                        normToDataMat_.m11,
                        normToDataMat_.m22);
    }

    inline Vector3f GetUserScale() const
    {
        return Vector3f(spaceToUserMat_.m00,
                        spaceToUserMat_.m11,
                        spaceToUserMat_.m22);
    }

//data properties
    inline const DataBbox& GetDataExtent() const {
        return dataExtent_;
    }

    inline Vector3i GetDataDimensions() const {
        return dataExtent_.getUnitDimensions();
    }

    void SetDataDimensions(const Vector3i& dim)
    {
        dataExtent_ = DataBbox(Vector3i::ZERO,
                               dim - Vector3i::ONE);
        update();
    }

    inline const Vector3f& GetDataResolution() const {
        return dataResolution_;
    }

    void SetDataResolution(const Vector3f& res)
    {
        dataResolution_ = res;
        update();
    }

    inline Vector2f GetStretchValues(const ViewType plane) const
    {
        switch(plane){
        case XY_VIEW:
            return Vector2f(dataStretchValues_.x, dataStretchValues_.y);
        case YZ_VIEW:
            return Vector2f(dataStretchValues_.z, dataStretchValues_.y);
        case XZ_VIEW:
            return Vector2f(dataStretchValues_.x, dataStretchValues_.z);
        }

        throw OmArgException("invalid type");
    }

// chunk dims
    inline int GetChunkDimension() const {
        return chunkDim_;
    }

    void SetChunkDimension(const int dim) {
        chunkDim_ = dim;
    }

// units
    const QString& GetUnit() const {
        return unitString_;
    }

    void SetUnit(const QString& unit) {
        unitString_ = unit;
    }

// coordinate frame methods

    /**
     *	Converts DataCoord to NormCoord based on source extent of data volume.
     *	NormCoords are used for relative location information and not absolute data access.
     *	centered : causes normalized coordinates to represent center of rectangular pixel.
     */
    inline NormCoord DataToNormCoord(const DataCoord& data,
                                     const bool centered = true) const
    {
        const Vector3f scale = dataExtent_.getUnitDimensions();
        const Vector3f offset = centered ?
            Vector3f(0.5f, 0.5f, 0.5f) :
            Vector3f::ZERO;
        return NormCoord((offset + data) / scale);
    }

    /**
     *	Returns rectangular pixel that contains given normalized coordinate.
     */
    inline DataCoord NormToDataCoord(const NormCoord& norm) const
    {
        const Vector3i scale = dataExtent_.getUnitDimensions();
        return DataCoord(floor(norm.x * scale.x),
                         floor(norm.y * scale.y),
                         floor(norm.z * scale.z));
    }


    /**
     *	Returns normalized bbox that encloses rectangular pixels in given data bbox.
     */
    inline NormBbox DataToNormBbox(const DataBbox& dataBbox) const
    {
        return NormBbox(DataToNormCoord(dataBbox.getMin(), false),
                        DataToNormCoord(dataBbox.getMax() + Vector3i::ONE, false));
    }

    /**
     *	Returns data bbox contained by given normalized bounding box.
     */
    inline DataBbox NormToDataBbox(const NormBbox& normBbox) const
    {
        const Vector3f normalized_pixel_dim =
            Vector3f::ONE / dataExtent_.getUnitDimensions();

        const NormCoord extent_min = normBbox.getMin() + normalized_pixel_dim * 0.5f;
        const NormCoord extent_max = normBbox.getMax() - normalized_pixel_dim * 0.5f;

        return DataBbox(NormToDataCoord(extent_min),
                        NormToDataCoord(extent_max));
    }

    inline const Matrix4f & GetNormToDataMatrix() const {
        return normToDataMat_;
    }

    inline const Matrix4f & GetNormToDataInvMatrix() const {
        return normToDataInvMat_;
    }

protected:
    static const int DefaultChunkDim = 128;

    OmVolCoords()
        : spaceToUserMat_(Matrix4f::IDENTITY)
        , spaceToUserInvMat_(Matrix4f::IDENTITY)
        , normToDataMat_(Matrix4f::IDENTITY)
        , normToDataInvMat_(Matrix4f::IDENTITY)
        , dataResolution_(Vector3f::ONE)
        , chunkDim_(DefaultChunkDim)
        , unitString_("")
    {
        SetDataDimensions(Vector3i(DefaultChunkDim,
                                   DefaultChunkDim,
                                   DefaultChunkDim));
        setUserScale(Vector3i(1, 1, 1));
    }

    virtual ~OmVolCoords()
    {}

private:
    bool update()
    {
        //update scale
        const Vector3i data_dims = dataExtent_.getUnitDimensions();
        setStretchValues();
        return setScale(dataResolution_ * data_dims);
    }

    bool setUserScale(const Vector3f& scale)
    {
        //set scale
        spaceToUserMat_.m[0][0] = scale.x;
        spaceToUserMat_.m[1][1] = scale.y;
        spaceToUserMat_.m[2][2] = scale.z;

        update();

        //set inverse and return if invertable
        return spaceToUserMat_.getInverse(spaceToUserInvMat_);
    }

    void setStretchValues()
    {
        const Vector3f res = dataResolution_;

        if( (res.x<=res.y) && (res.x<=res.z)){
            dataStretchValues_.x = 1.0;
            dataStretchValues_.y = res.y/res.x;
            dataStretchValues_.z = res.z/res.x;
        } else {
            if (res.y<=res.z){
                dataStretchValues_.x = res.x/res.y;
                dataStretchValues_.y = 1.0;
                dataStretchValues_.z = res.z/res.y;
            } else {
                dataStretchValues_.x = res.x/res.z;
                dataStretchValues_.y = res.y/res.z;
                dataStretchValues_.z = 1.0;
            }
        }
    }

    bool setScale(const Vector3f& scale)
    {
        //set scale
        normToDataMat_.m[0][0] = scale.x;
        normToDataMat_.m[1][1] = scale.y;
        normToDataMat_.m[2][2] = scale.z;

        //set inverse and return if invertable
        return normToDataMat_.getInverse(normToDataInvMat_);
    }
};

#endif

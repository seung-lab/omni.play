#pragma once

#include "common/common.h"
#include "zi/concurrency.hpp"

namespace om {
namespace coords {

class globalBbox;
class chunkCoord;

class volumeSystem {
protected:
    Matrix4f dataToGlobal_;
    Matrix4f globalToData_;
    Matrix4f normToGlobal_;
    Matrix4f globalToNorm_;

    //data properties
    Vector3i dataDimensions_;

    int chunkDim_;
    int mMipRootLevel;

public:
    static const int DefaultChunkDim = 128;

    volumeSystem();

    void UpdateRootLevel();

    inline int GetRootMipLevel() const {
        return mMipRootLevel;
    }

    inline Vector3i MipedDataDimensions(const int level) const
    {
        return GetDataDimensions() / math::pow2int(level);
    }

    // Calculate the data dimensions needed to contain the volume at a given compression level.
    // TODO: should this be factored out?
    inline Vector3i MipLevelDataDimensions(const int level) const
    {
        //get dimensions
        globalBbox source_extent = GetDataExtent();
        Vector3f source_dims = source_extent.getUnitDimensions();

        //dims in fraction of pixels
        Vector3f mip_level_dims = source_dims / math::pow2int(level);

        return Vector3i(ceil(mip_level_dims.x),
                        ceil(mip_level_dims.y),
                        ceil(mip_level_dims.z));
    }


    //data properties
    globalBbox GetDataExtent() const;

    inline Vector3i GetDataDimensions() const {
        return dataDimensions_;
    }

    inline void SetDataDimensions(const Vector3f& dim)
    {
        dataDimensions_ = dim;

        Vector3i dims = GetDataDimensions();
        normToGlobal_.m00 = dims.x;
        normToGlobal_.m11 = dims.y;
        normToGlobal_.m22 = dims.z;
        normToGlobal_.getInverse(globalToNorm_);
    }

// chunk dims
    inline int GetChunkDimension() const {
        return chunkDim_;
    }

    inline void SetChunkDimension(const int dim) {
        chunkDim_ = dim;
    }

    inline Vector3i GetChunkDimensions() const
    {
        return Vector3i(GetChunkDimension(),
                        GetChunkDimension(),
                        GetChunkDimension());
    }

// coordinate frame methods
    inline Matrix4f DataToGlobalMat(int mipLevel = 0) const {
        Matrix4f ret = dataToGlobal_;
        int factor = math::pow2int(mipLevel);
        ret.scaleTranslation(1.0f / factor);
        ret.m33 = 1.0f / factor;
        return ret;
    }

    inline Matrix4f GlobalToDataMat(int mipLevel = 0) const {
        Matrix4f ret = globalToData_;
        int factor = math::pow2int(mipLevel);
        ret.m33 = factor;
        return ret;
    }

    inline const Matrix4f& NormToGlobalMat() const {
        return normToGlobal_;
    }

    inline const Matrix4f& GlobalToNormMat() const {
        return globalToNorm_;
    }

    inline Vector3i GetAbsOffset() const {
        return dataToGlobal_.getTranslation();
    }

    inline void SetAbsOffset(Vector3i absOffset) {
        dataToGlobal_.setTranslation(absOffset);
        dataToGlobal_.getInverse(globalToData_);
        normToGlobal_.setTranslation(absOffset);
        normToGlobal_.getInverse(globalToNorm_);
    }

    inline Vector3i GetResolution() const {
        return Vector3i(dataToGlobal_.m00, dataToGlobal_.m11, dataToGlobal_.m22);
    }

    inline void SetResolution(Vector3i resolution) {
        dataToGlobal_.m00 = resolution.x;
        dataToGlobal_.m11 = resolution.y;
        dataToGlobal_.m22 = resolution.z;

        dataToGlobal_.getInverse(globalToData_);
    }


    inline Vector3i getDimsRoundedToNearestChunk(const int level) const
    {
        const Vector3i data_dims = MipLevelDataDimensions(level);

        return Vector3i(math::roundUp(data_dims.x, GetChunkDimension()),
                        math::roundUp(data_dims.y, GetChunkDimension()),
                        math::roundUp(data_dims.z, GetChunkDimension()));
    }

    //mip chunk methods
    chunkCoord RootMipChunkCoordinate() const;

    // Returns true if given MipCoordinate is a valid coordinate within the MipVolume.
    bool ContainsMipChunkCoord(const chunkCoord & rMipCoord) const;
};

} // namespace coords
}

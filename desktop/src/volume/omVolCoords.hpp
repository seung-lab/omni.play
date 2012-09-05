#pragma once

/*
 * OmVolume is the abstract represntation of multiple forms of data enclosed by
 *   a region of space.
 *
 *  Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "common/omCommon.h"
#include "events/omEvents.h"
#include "zi/concurrency.hpp"

class OmVolCoords {
protected:
    Matrix4f dataToGlobal_;
    Matrix4f globalToData_;
    Matrix4f normToGlobal_;
    Matrix4f globalToNorm_;

    //data properties
    Vector3i dataDimensions_;
    Vector3i resolution_;

    int chunkDim_;

public:

    //data properties
    inline om::globalBbox GetDataExtent() const
    {
        Vector3f abs = GetAbsOffset();
        return om::globalBbox(abs, abs - Vector3f::ONE + dataDimensions_);
    }

    inline Vector3i GetDataDimensions() const {
        return dataDimensions_;
    }

    void SetDataDimensions(const Vector3f& dim)
    {
        dataDimensions_ = dim;
        updateNormMat();
    }

// chunk dims
    inline int GetChunkDimension() const {
        return chunkDim_;
    }

    void SetChunkDimension(const int dim) {
        chunkDim_ = dim;
    }

// coordinate frame methods

    inline Matrix4f DataToGlobalMat() const {
        return dataToGlobal_;
    }

    inline Matrix4f GlobalToDataMat() const {
        return globalToData_;
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
    	resolution_ = resolution;

        dataToGlobal_.m00 = resolution.x;
        dataToGlobal_.m11 = resolution.y;
        dataToGlobal_.m22 = resolution.z;

        dataToGlobal_.getInverse(globalToData_);
        updateNormMat();
    }

protected:
    static const int DefaultChunkDim = 128;

    OmVolCoords()
        : dataToGlobal_(Matrix4f::IDENTITY)
        , globalToData_(Matrix4f::IDENTITY)
        , normToGlobal_(Matrix4f::IDENTITY)
        , globalToNorm_(Matrix4f::IDENTITY)
        , chunkDim_(DefaultChunkDim)
    {
        SetDataDimensions(Vector3i(DefaultChunkDim,
                                   DefaultChunkDim,
                                   DefaultChunkDim));
    }

    virtual ~OmVolCoords()
    {}

    void updateNormMat()
    {
        normToGlobal_.m00 = dataDimensions_.x * resolution_.x;
        normToGlobal_.m11 = dataDimensions_.y * resolution_.y;
        normToGlobal_.m22 = dataDimensions_.z * resolution_.z;
        normToGlobal_.getInverse(globalToNorm_);
    }
};


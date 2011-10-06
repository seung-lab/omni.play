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
    int mMipLeafDim;
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
        return GetDataDimensions() / om::math::pow2int(level);
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
        int factor = om::math::pow2int(mipLevel);
        ret.scaleTranslation(1.0f / factor);
        ret.m33 = 1.0f / factor;
        return ret;
    }

    inline Matrix4f GlobalToDataMat(int mipLevel = 0) const {
        Matrix4f ret = globalToData_;
        int factor = om::math::pow2int(mipLevel);
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
    
    //mip chunk methods
    chunkCoord RootMipChunkCoordinate() const;
    
    // Returns true if given MipCoordinate is a valid coordinate within the MipVolume.
    bool ContainsMipChunkCoord(const chunkCoord & rMipCoord) const;
};

} // namespace coords
}
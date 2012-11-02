#pragma once

#include "common/common.h"
#include "zi/concurrency.hpp"

namespace om {
namespace coords {

class GlobalBbox;
class Chunk;

class VolumeSystem {
protected:
    Matrix4f dataToGlobal_;
    Matrix4f globalToData_;
    Matrix4f normToGlobal_;
    Matrix4f globalToNorm_;

    //data properties
    Vector3i dataDimensions_;

    int chunkDimension_;
    int mMipRootLevel;

public:
    static const int DefaultChunkDim = 128;

    VolumeSystem();
    VolumeSystem(Vector3i dims, Vector3i abs = Vector3i::ZERO, Vector3i res = Vector3i::ONE, uint chunkDim = DefaultChunkDim)
    	: dataToGlobal_(Matrix4f::IDENTITY)
        , globalToData_(Matrix4f::IDENTITY)
        , normToGlobal_(Matrix4f::IDENTITY)
        , globalToNorm_(Matrix4f::IDENTITY)
        , chunkDimension_(DefaultChunkDim)
        , mMipRootLevel(0)
    {
    	SetDataDimensions(dims);
        SetAbsOffset(abs);
        SetResolution(res);
        SetChunkDimension(chunkDim);
        UpdateRootLevel();
    }

    void UpdateRootLevel();

    inline int RootMipLevel() const {
        return mMipRootLevel;
    }

    inline Vector3i MipedDataDimensions(const int level) const
    {
        return DataDimensions() / math::pow2int(level);
    }

    inline Vector3i MipLevelDataDimensions(const int level) const {
        return Extent().ToDataBbox(this, level).getMax();
     }

    inline Vector3i MipLevelDimensionsInMipChunks(int level) const
    {
        const Vector3f data_dims = MipLevelDataDimensions(level);
        return Vector3i (ceil(data_dims.x / ChunkDimension()),
                         ceil(data_dims.y / ChunkDimension()),
                         ceil(data_dims.z / ChunkDimension()));
    }

    //data properties
    GlobalBbox Extent() const;

    inline Vector3i DataDimensions() const {
        return dataDimensions_;
    }

    inline void SetDataDimensions(const Vector3f& dim)
    {
        dataDimensions_ = dim;

        Vector3i dims = DataDimensions();
        normToGlobal_.m00 = dims.x;
        normToGlobal_.m11 = dims.y;
        normToGlobal_.m22 = dims.z;
        normToGlobal_.getInverse(globalToNorm_);
    }

// Chunk dims
    inline int ChunkDimension() const {
        return chunkDimension_;
    }

    inline void SetChunkDimension(const int dim) {
        chunkDimension_ = dim;
    }

    inline Vector3i ChunkDimensions() const
    {
        return Vector3i(ChunkDimension(),
                        ChunkDimension(),
                        ChunkDimension());
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

    inline Vector3i AbsOffset() const {
        return dataToGlobal_.getTranslation();
    }

    inline void SetAbsOffset(Vector3i absOffset) {
        dataToGlobal_.setTranslation(absOffset);
        dataToGlobal_.getInverse(globalToData_);
        normToGlobal_.setTranslation(absOffset);
        normToGlobal_.getInverse(globalToNorm_);
    }

    inline Vector3i Resolution() const {
        return Vector3i(dataToGlobal_.m00, dataToGlobal_.m11, dataToGlobal_.m22);
    }

    inline void SetResolution(Vector3i resolution) {
        dataToGlobal_.m00 = resolution.x;
        dataToGlobal_.m11 = resolution.y;
        dataToGlobal_.m22 = resolution.z;

        dataToGlobal_.getInverse(globalToData_);
    }


    inline Vector3i DimsRoundedToNearestChunk(const int level) const
    {
        const Vector3i data_dims = MipLevelDataDimensions(level);

        return Vector3i(math::roundUp(data_dims.x, ChunkDimension()),
                        math::roundUp(data_dims.y, ChunkDimension()),
                        math::roundUp(data_dims.z, ChunkDimension()));
    }

    //mip Chunk methods
    Chunk RootMipChunkCoordinate() const;

    boost::shared_ptr<std::vector<coords::Chunk> > MipChunkCoords() const;
    boost::shared_ptr<std::vector<coords::Chunk> > MipChunkCoords(const int mipLevel) const;

    // Returns true if given MipCoordinate is a valid coordinate within the MipVolume.
    bool ContainsMipChunk(const Chunk & rMipCoord) const;
private:
    void addChunkCoordsForLevel(const int mipLevel, std::vector<coords::Chunk>* coords) const;

};

} // namespace coords
}

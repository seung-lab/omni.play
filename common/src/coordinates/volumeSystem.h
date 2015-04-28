#pragma once

#include "common/common.h"
#include "coordinates/coordinates.h"

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

  Vector3i dataDimensions_;
  Vector3i chunkDimensions_;
  Vector3i resolution_;

  int rootMipLevel_;

 public:
  static const int DefaultChunkDim = 128;

  VolumeSystem();

  VolumeSystem(Vector3i dims, Vector3i abs = Vector3i::ZERO,
               Vector3i res = Vector3i::ONE, uint chunkDim = DefaultChunkDim)
      : dataToGlobal_(Matrix4f::IDENTITY),
        globalToData_(Matrix4f::IDENTITY),
        normToGlobal_(Matrix4f::IDENTITY),
        globalToNorm_(Matrix4f::IDENTITY),
        chunkDimensions_(Vector3i(chunkDim)),
        resolution_(Vector3i::ONE),
        rootMipLevel_(0) {
    SetDataDimensions(dims);
    SetAbsOffset(abs);
    SetResolution(res);
    UpdateRootLevel();
  }

  void UpdateRootLevel();

  inline int RootMipLevel() const { return rootMipLevel_; }

  inline Vector3i MipedDataDimensions(const int level) const {
    return DataDimensions() / math::pow2int(level);
  }

  Vector3i MipLevelDataDimensions(const int level) const;

  inline Vector3i MipLevelDimensionsInMipChunks(int level) const {
    const Vector3f data_dims = MipLevelDataDimensions(level);
    return Vector3i(ceil(data_dims.x / chunkDimensions_.x),
                    ceil(data_dims.y / chunkDimensions_.y),
                    ceil(data_dims.z / chunkDimensions_.z));
  }

  // data properties
  GlobalBbox Extent() const;

  bool Contains(coords::Global g) const { return Extent().contains(g); }
  bool Contains(coords::Data dc) const {
    const auto mippedDims = MipedDataDimensions(dc.mipLevel());
    return dc.x < mippedDims.x && dc.y < mippedDims.y && dc.z < mippedDims.z &&
           dc.x >= 0 && dc.y >= 0 && dc.z >= 0;
  }

  inline Vector3i DataDimensions() const { return dataDimensions_; }

  inline void SetDataDimensions(const Vector3f& dim) {
    dataDimensions_.x = om::math::roundUp((int)dim.x, chunkDimensions_.x);
    dataDimensions_.y = om::math::roundUp((int)dim.y, chunkDimensions_.y);
    dataDimensions_.z = om::math::roundUp((int)dim.z, chunkDimensions_.z);

    updateNormMat();
  }

  inline void SetChunkDimensions(const Vector3i& dims) {
    chunkDimensions_ = dims;
  }

  inline Vector3i ChunkDimensions() const { return chunkDimensions_; }

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

  inline const Matrix4f& NormToGlobalMat() const { return normToGlobal_; }

  inline const Matrix4f& GlobalToNormMat() const { return globalToNorm_; }

  inline Vector3i AbsOffset() const { return dataToGlobal_.getTranslation(); }

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
    resolution_ = resolution;

    dataToGlobal_.m00 = resolution.x;
    dataToGlobal_.m11 = resolution.y;
    dataToGlobal_.m22 = resolution.z;

    dataToGlobal_.getInverse(globalToData_);
    updateNormMat();
  }

  inline Vector3i DimsRoundedToNearestChunk(const int level) const {
    const Vector3i data_dims = MipLevelDataDimensions(level);

    return Vector3i(math::roundUp(data_dims.x, chunkDimensions_.x),
                    math::roundUp(data_dims.y, chunkDimensions_.y),
                    math::roundUp(data_dims.z, chunkDimensions_.z));
  }

  inline void ValidMipChunkCoordChildren(const Chunk& coord,
                                         std::set<Chunk>& children) const {
    // clear set
    children.clear();

    // get all possible children
    std::vector<Chunk> possible_children = coord.ChildrenCoords();

    // for all possible children
    for (auto& chunk : possible_children) {
      if (ContainsMipChunk(chunk)) {
        children.insert(chunk);
      }
    }
  }

  inline uint32_t ComputeTotalNumChunks() const {
    uint32_t numChunks = 0;

    for (auto level = 0; level <= rootMipLevel_; ++level) {
      numChunks += ComputeTotalNumChunks(level);
    }

    return numChunks;
  }

  inline uint32_t ComputeTotalNumChunks(const int mipLevel) const {
    const Vector3i dims = MipLevelDimensionsInMipChunks(mipLevel);
    return dims.x * dims.y * dims.z;
  }

  Chunk RootMipChunkCoordinate() const;

  std::shared_ptr<std::vector<coords::Chunk>> MipChunkCoords() const;
  std::shared_ptr<std::vector<coords::Chunk>> MipChunkCoords(const int mipLevel)
      const;

  bool ContainsMipChunk(const Chunk& rMipCoord) const;

  inline uint32_t GetNumberOfVoxelsPerChunk() const {
    return chunkDimensions_.x * chunkDimensions_.y * chunkDimensions_.z;
  }

 private:
  void addChunkCoordsForLevel(const int mipLevel,
                              std::vector<coords::Chunk>* coords) const;

  void updateNormMat() {
    normToGlobal_.m00 = dataDimensions_.x * resolution_.x;
    normToGlobal_.m11 = dataDimensions_.y * resolution_.y;
    normToGlobal_.m22 = dataDimensions_.z * resolution_.z;
    normToGlobal_.getInverse(globalToNorm_);
  }
};

}  // namespace coords
}

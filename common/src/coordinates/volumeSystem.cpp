#include "common/common.h"
#include "zi/concurrency.hpp"
#include "coordinates/volumeSystem.h"

namespace om {
namespace coords {

VolumeSystem::VolumeSystem()
    : dataToGlobal_(Matrix4f::IDENTITY),
      globalToData_(Matrix4f::IDENTITY),
      normToGlobal_(Matrix4f::IDENTITY),
      globalToNorm_(Matrix4f::IDENTITY),
      chunkDimensions_(Vector3i(DefaultChunkDim)),
      resolution_(Vector3i::ONE),
      rootMipLevel_(0) {
  SetDataDimensions(Vector3i(DefaultChunkDim));

  UpdateRootLevel();
}

void VolumeSystem::UpdateRootLevel() {
  float ratio =
      (Vector3f(DataDimensions()) / chunkDimensions_).getMaxComponent();
  if (ratio <= 1) {
    rootMipLevel_ = 0;
  } else {
    rootMipLevel_ = ceil(log(ratio) / log(2.0f));
  }
}

GlobalBbox VolumeSystem::Extent() const {
  Vector3f abs = AbsOffset();
  return GlobalBbox(abs, abs + (DataDimensions()-Vector3f::ONE) * Resolution());
}

DataBbox VolumeSystem::Bounds() const { return Extent().ToDataBbox(*this, 0); }

bool VolumeSystem::ContainsMipChunk(const Chunk& rMipCoord) const {
  // if level is greater than root level
  if (rMipCoord.mipLevel() < 0 || rMipCoord.mipLevel() > RootMipLevel()) {
    return false;
  }

  // convert to data box in leaf (MIP 0)
  GlobalBbox bbox = rMipCoord.BoundingBox(*this).ToGlobalBbox();

  bbox.intersect(Extent());
  if (bbox.isEmpty()) {
    return false;
  }

  // else valid mip coord
  return true;
}

Chunk VolumeSystem::RootMipChunkCoordinate() const {
  return Chunk(rootMipLevel_, Vector3i::ZERO);
}

std::shared_ptr<std::vector<coords::Chunk>> VolumeSystem::MipChunkCoords()
    const {
  auto ret = std::make_shared<std::vector<coords::Chunk>>();
  for (auto level = 0; level <= rootMipLevel_; ++level) {
    addChunkCoordsForLevel(level, ret.get());
  }
  return ret;
}

std::shared_ptr<std::vector<coords::Chunk>> VolumeSystem::MipChunkCoords(
    const int mipLevel) const {
  auto ret = std::make_shared<std::vector<coords::Chunk>>();
  addChunkCoordsForLevel(mipLevel, ret.get());
  return ret;
}

void VolumeSystem::addChunkCoordsForLevel(
    const int mipLevel, std::vector<coords::Chunk>* coords) const {
  const Vector3i dims = MipLevelDimensionsInMipChunks(mipLevel);
  coords->reserve(coords->size() + dims.x * dims.y * dims.z);
  for (auto z = 0; z < dims.z; ++z) {
    for (auto y = 0; y < dims.y; ++y) {
      for (auto x = 0; x < dims.x; ++x) {
        coords->push_back(coords::Chunk(mipLevel, x, y, z));
      }
    }
  }
}

}  // namespace coords
}  // namespace om

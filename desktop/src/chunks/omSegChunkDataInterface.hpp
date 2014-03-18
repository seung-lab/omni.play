#pragma once
#include "precomp.h"

#include "coordinates/data.h"
#include "volume/omVolumeTypes.hpp"

namespace om {
class dataCoord;
}
class OmSegChunk;
class OmSegments;
class OmSegmentation;

namespace om {
namespace segchunk {

class dataInterface {
 public:
  virtual void ProcessChunk(const bool, OmSegments* segments) = 0;

  virtual void RefreshBoundingData(OmSegments* segments) = 0;

  virtual std::shared_ptr<uint32_t> ExtractDataSlice32bit(
      const om::common::ViewType, const int) = 0;
  virtual std::shared_ptr<uint32_t> GetCopyOfChunkDataAsUint32() = 0;

  virtual void RewriteChunk(const std::unordered_map<uint32_t, uint32_t>&) = 0;

  virtual uint32_t SetVoxelValue(const om::coords::Data& voxel,
                                 const uint32_t val) = 0;
  virtual uint32_t GetVoxelValue(const om::coords::Data& voxel) = 0;
};

}  // namespace segchunk
}  // namespace om

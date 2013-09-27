#pragma once

#include "tiles/cache/raw/omRawSegTileCacheTypes.hpp"
#include "volume/omVolumeTypes.hpp"

class OmSegChunk;
class OmSegments;
class OmSegmentation;

namespace om {
namespace segchunk {

class dataInterface {
public:
    virtual void ProcessChunk(const bool, OmSegments* segments) = 0;

    virtual void RefreshBoundingData(OmSegments* segments) = 0;

    virtual PooledTile32Ptr ExtractDataSlice32bit(const om::common::ViewType, const int) = 0;
    virtual std::shared_ptr<uint32_t> GetCopyOfChunkDataAsUint32() = 0;

    virtual void RewriteChunk(const std::unordered_map<uint32_t, uint32_t>&) = 0;

    virtual uint32_t SetVoxelValue(const om::dataCoord& voxel, const uint32_t val) = 0;
    virtual uint32_t GetVoxelValue(const om::dataCoord& voxel) = 0;
};

} // namespace segchunk
} // namespace om



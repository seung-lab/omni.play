#pragma once

#include "tiles/cache/raw/omRawSegTileCacheTypes.hpp"
#include "volume/volumeTypes.h"

class segChunk;
class segments;
class segmentation;

namespace om {
namespace segchunk {

class dataInterface {
public:
    virtual void ProcessChunk(const bool, segments* segments) = 0;

    virtual void RefreshBoundingData(segments* segments) = 0;

    virtual PooledTile32Ptr ExtractDataSlice32bit(const common::viewType, const int) = 0;
    virtual boost::shared_ptr<uint32_t> GetCopyOfChunkDataAsUint32() = 0;

    virtual void RewriteChunk(const boost::unordered_map<uint32_t, uint32_t>&) = 0;

    virtual uint32_t SetVoxelValue(const coords::dataCoord& voxel, const uint32_t val) = 0;
    virtual uint32_t GetVoxelValue(const coords::dataCoord& voxel) = 0;
};

} // namespace segchunk
} // namespace om



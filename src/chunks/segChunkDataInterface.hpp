#pragma once

#include "volume/volumeTypes.h"

namespace om {
namespace segmet {
class segChunk;
class segments;
}
namespace volume { class segmentation; }

namespace segchunk {

class dataInterface {
public:
    virtual void ProcessChunk(const bool, segment::segments* segments) = 0;

    virtual void RefreshBoundingData(segment::segments* segments) = 0;

    virtual boost::shared_ptr<uint32_t> ExtractDataSlice32bit(const common::viewType, const int) = 0;
    virtual boost::shared_ptr<uint32_t> GetCopyOfChunkDataAsUint32() = 0;

    virtual void RewriteChunk(const boost::unordered_map<uint32_t, uint32_t>&) = 0;

    virtual uint32_t SetVoxelValue(const coords::dataCoord& voxel, const uint32_t val) = 0;
    virtual uint32_t GetVoxelValue(const coords::dataCoord& voxel) = 0;
};

} // namespace segchunk
} // namespace om



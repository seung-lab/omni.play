#pragma once

#include "volume/volumeTypes.h"

namespace om {
namespace volume { class segmentation; }
namespace segments { class segments; }
namespace chunks { class segChunk; }

namespace segchunk {

class dataInterface {
public:
    virtual boost::shared_ptr<char> ExtractDataSlice32bit(const common::viewType, const int) = 0;

    virtual uint32_t SetVoxelValue(const coords::dataCoord& voxel, const uint32_t val) = 0;
    virtual uint32_t GetVoxelValue(const coords::dataCoord& voxel) = 0;
};

} // namespace segchunk
} // namespace om



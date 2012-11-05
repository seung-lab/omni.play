#pragma once

#include "common/om.hpp"
#include "common/common.h"

// for Microsft Windows compiler compatibility, may need #pragma pack
// see http://gcc.gnu.org/onlinedocs/gcc/Structure_002dPacking-Pragmas.html

struct OmSegmentDataV2 {
    om::common::SegID value;
    om::common::Color color;
    bool immutable;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct OmSegmentDataV3 {
    om::common::SegID value;
    om::common::Color color;
    om::common::SegListType listType;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct OmSegmentDataV4 {
    om::common::SegID value;
    om::common::Color color;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};


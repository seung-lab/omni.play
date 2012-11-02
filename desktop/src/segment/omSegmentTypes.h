#pragma once

#include "common/om.hpp"
#include "common/common.h"

// for Microsft Windows compiler compatibility, may need #pragma pack
// see http://gcc.gnu.org/onlinedocs/gcc/Structure_002dPacking-Pragmas.html

struct OmSegmentDataV2 {
    OmSegID value;
    om::common::Color color;
    bool immutable;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct OmSegmentDataV3 {
    OmSegID value;
    om::common::Color color;
    om::SegListType listType;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct OmSegmentDataV4 {
    OmSegID value;
    om::common::Color color;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};


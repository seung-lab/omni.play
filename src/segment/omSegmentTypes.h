#pragma once

#include "common/om.hpp"
#include "common/common.h"

// for Microsft Windows compiler compatibility, may need #pragma pack
// see http://gcc.gnu.org/onlinedocs/gcc/Structure_002dPacking-Pragmas.html

struct OmSegmentDataV2 {
    segId value;
    OmColor color;
    bool immutable;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct OmSegmentDataV3 {
    segId value;
    OmColor color;
    om::SegListType listType;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct OmSegmentDataV4 {
    segId value;
    OmColor color;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};


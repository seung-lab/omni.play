#pragma once

#include "common/om.hpp"
#include "common/omCommon.h"

// for Microsft Windows compiler compatibility, may need #pragma pack
// see http://gcc.gnu.org/onlinedocs/gcc/Structure_002dPacking-Pragmas.html

struct OmSegmentDataV2 {
    OmSegID value;
    OmColor color;
    bool immutable;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct OmSegmentDataV3 {
    OmSegID value;
    OmColor color;
    om::SegListType listType;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct OmSegmentDataV4 {
    OmSegID value;
    OmColor color;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};


#pragma once

#include "common/om.hpp"
#include "common/common.h"

// for Microsft Windows compiler compatibility, may need #pragma pack
// see http://gcc.gnu.org/onlinedocs/gcc/Structure_002dPacking-Pragmas.html

struct segmentDataV2 {
    segId value;
    OmColor color;
    bool immutable;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct segmentDataV3 {
    segId value;
    OmColor color;
    om::SegListType listType;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};

struct segmentDataV4 {
    segId value;
    OmColor color;
    uint64_t size;
    AxisAlignedBoundingBox<int> bounds;
};


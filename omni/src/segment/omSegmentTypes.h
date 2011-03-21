#ifndef OM_SEGMENT_TYPES_H
#define OM_SEGMENT_TYPES_H

#include "common/om.hpp"
#include "common/omCommon.h"

struct OmSegmentDataV2 {
    OmSegID value;
    OmColor color;
    bool immutable;
    uint64_t size;
    DataBbox bounds;
};

struct OmSegmentDataV3 {
    OmSegID value;
    OmColor color;
    om::SegListType listType;
    uint64_t size;
    DataBbox bounds;
};

struct OmSegmentDataV4 {
    OmSegID value;
    OmColor color;
    uint64_t size;
    DataBbox bounds;
};

#endif

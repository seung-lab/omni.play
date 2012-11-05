#pragma once

#include "common/common.h"

class OmViewGroupState;
class OmSegments;

class SegmentColorParams {
public:
    om::common::SegmentColorCacheType sccType;
    uint32_t numElements;
    OmViewGroupState* vgs;
    OmSegments* segments;
};

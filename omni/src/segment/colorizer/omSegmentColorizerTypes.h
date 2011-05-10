#pragma once

#include "common/omCommon.h"

class OmViewGroupState;
class OmSegments;

struct SegmentColorParams {
    OmSegmentColorCacheType sccType;
    uint32_t numElements;
    OmViewGroupState* vgs;
    OmSegments* segments;
};


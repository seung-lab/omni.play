#ifndef OM_SEGEMNT_COLORIZER_TYPES_H
#define OM_SEGEMNT_COLORIZER_TYPES_H

#include "common/omCommon.h"

class OmViewGroupState;
class OmSegments;

struct SegmentColorParams {
    OmSegmentColorCacheType sccType;
    uint32_t numElements;
    OmViewGroupState* vgs;
    OmSegments* segments;
};

#endif

#pragma once

#include "common/omCommon.h"

class OmViewGroupState;
class OmSegments;

class SegmentColorParams {
 public:
  OmSegmentColorCacheType sccType;
  uint32_t numElements;
  OmViewGroupState* vgs;
  OmSegments* segments;
};

#pragma once
#include "precomp.h"

#include "common/common.h"
#include "segment/coloring.hpp"

class OmViewGroupState;
class OmSegments;

class SegmentColorParams {
 public:
  om::segment::coloring sccType;
  uint32_t numElements;
  OmViewGroupState* vgs;
  OmSegments* segments;
};

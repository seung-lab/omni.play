#pragma once

#include "common/common.h"
#include "common/enums.hpp"
#include "segment/coloring.hpp"

class OmViewGroupState;
class OmSegments;
namespace om {
namespace segment {
class Selection;
}
}

class SegmentColorParams {
 public:
  om::segment::coloring sccType;
  uint32_t numElements;
  OmViewGroupState& vgs;
  OmSegments& segments;
  om::segment::Selection& selection;
};

#pragma once
#include "precomp.h"

#include "viewGroup/omViewGroupState.h"
#include "utility/segmentationDataWrapper.hpp"
#include "common/enums.hpp"
class SegmentDataWrapper;
class OmViewGroupState;

namespace om {
namespace coords {
  class Global;
}

namespace JoinSplitRunner {
  void FindAndPerformOnSegments(const SegmentDataWrapper curSDW,
                                   OmViewGroupState& vgs,
                                   const om::coords::Global curClickPt,
                                   om::tool::mode tool);
}
}

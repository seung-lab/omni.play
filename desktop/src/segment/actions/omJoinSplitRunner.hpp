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
  void GoToNextState(OmViewGroupState& vgs,om::tool::mode tool);
  void SelectSegment(OmViewGroupState& vgs, const om::tool::mode tool,
    const SegmentDataWrapper segmentDataWrapper);
}
}

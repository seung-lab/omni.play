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
  /* 
   * Tell joinningSplitting to go to the next state. don't go to the next state
   * if there is nothing selected.
   */
  bool GoToNextState(OmViewGroupState& vgs, const om::tool::mode tool);
  bool SelectSegment(OmViewGroupState& vgs, const om::tool::mode tool,
    boost::optional<SegmentDataWrapper> segmentDataWrapper);
}
}

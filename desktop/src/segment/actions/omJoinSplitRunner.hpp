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
  bool GoToNextState(OmViewGroupState& vgs, const om::tool::mode tool);
  bool SelectSegment(OmViewGroupState& vgs, const om::tool::mode tool,
    boost::optional<SegmentDataWrapper> segmentDataWrapper);
}
}

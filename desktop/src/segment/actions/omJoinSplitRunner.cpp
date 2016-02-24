#pragma once
#include "precomp.h"

#include "segment/actions/omJoinSplitRunner.hpp"
#include "gui/tools.hpp"
#include "viewGroup/omJoiningSplitting.hpp"
#include "viewGroup/omViewGroupState.h"
#include "segment/omSegment.h"
#include "utility/segmentationDataWrapper.hpp"
#include "common/enums.hpp"

void om::JoinSplitRunner::FindAndPerformOnSegments(const SegmentDataWrapper curSDW,
                                 OmViewGroupState& vgs,
                                 const om::coords::Global curClickPt,
                                 om::tool::mode tool) {
  const boost::optional<om::coords::Global> prevClickPt =
      vgs.JoiningSplitting().FirstCoord();

  if (prevClickPt) {
    const SegmentDataWrapper prevSDW = vgs.JoiningSplitting().FirstSegment();

    OmSegment* seg1 = prevSDW.GetSegment();
    OmSegment* seg2 = curSDW.GetSegment();

    vgs.JoiningSplitting().DeactivateTool();

    if (!seg1 || !seg2) {
      return;
    }

    if (seg1 == seg2) {
      log_debugs << "can't perform action on same segment";
      return;
    }

    switch (tool) {
      case om::tool::mode::JOIN:
        OmActions::JoinSegments(vgs.Segmentation(), seg1, seg2);
        break;
      case om::tool::mode::SPLIT:
        OmActions::FindAndSplitSegments(vgs.Segmentation(), seg1, seg2);
        break;
    }

  } else {
    if (curSDW.IsSegmentValid()) {
      // set segment to be split later...
      vgs.JoiningSplitting().SetFirstPoint(tool, curSDW, curClickPt);
    }
  }
}

#pragma once
#include "precomp.h"

#include "viewGroup/omJoiningSplitting.hpp"
#include "viewGroup/omViewGroupState.h"
#include "utility/segmentationDataWrapper.hpp"
#include "common/enums.hpp"

class OmJoinSplitRunner {
 public:
  static void FindAndPerformOnSegments(const SegmentDataWrapper curSDW,
                                   OmViewGroupState& vgs,
                                   const om::coords::Global curClickPt,
                                   om::tool::mode tool) {
    const boost::optional<om::coords::Global> prevClickPt =
        vgs.JoiningSplitting().FirstCoord();

    if (prevClickPt) {
      const SegmentDataWrapper prevSDW = vgs.JoiningSplitting().FirstSegment();

      OmSegment* seg1 = prevSDW.GetSegment();
      OmSegment* seg2 = curSDW.GetSegment();

      vgs.JoiningSplitting().ExitFixButton();

      if (!seg1 || !seg2) {
        return;
      }

      if (seg1 == seg2) {
        log_debugs << "can't split--same segment";
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
        vgs.JoiningSplitting().SetFirstPoint(curSDW, curClickPt);
      }
    }
  }
};

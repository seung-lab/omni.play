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
								   om::common::JoinOrSplit joinOrSplit) {
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
      if (joinOrSplit == om::common::JoinOrSplit::JOIN) {
        OmActions::JoinSegments(vgs.Segmentation(), seg1, seg2);
      } else {
        OmActions::FindAndSplitSegments(vgs.Segmentation(), seg1, seg2);
      }

    } else {
      if (curSDW.IsSegmentValid()) {
        // set segment to be split later...
        vgs.JoiningSplitting().SetFirstPoint(curSDW, curClickPt);
      }
    }
  }
};

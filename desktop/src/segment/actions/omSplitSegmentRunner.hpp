#pragma once
#include "precomp.h"

#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omSplitting.hpp"
#include "utility/segmentationDataWrapper.hpp"

class OmSplitSegmentRunner {
 public:
  static void FindAndSplitSegments(const SegmentDataWrapper curSDW,
                                   OmViewGroupState& vgs,
                                   const om::coords::Global curClickPt) {
    const boost::optional<om::coords::Global> prevClickPt =
        vgs.TwoSegmentAction().FirstCoord();

    if (prevClickPt) {
      const SegmentDataWrapper prevSDW = vgs.TwoSegmentAction().FirstSegment();

      OmSegment* seg1 = prevSDW.GetSegment();
      OmSegment* seg2 = curSDW.GetSegment();

      vgs.Splitting().ExitSplitModeFixButton();

      if (!seg1 || !seg2) {
        return;
      }

      if (seg1 == seg2) {
        log_debugs << "can't split--same segment";
        return;
      }

      OmActions::FindAndSplitSegments(vgs.Segmentation(), seg1, seg2);

    } else {
      if (curSDW.IsSegmentValid()) {
        // set segment to be split later...
        vgs.TwoSegmentAction().SetFirstSplitPoint(curSDW, curClickPt);
      }
    }
  }
};

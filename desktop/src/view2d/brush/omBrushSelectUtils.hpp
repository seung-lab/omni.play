#pragma once
#include "precomp.h"

#include "events/events.h"
#include "segment/omSegmentSelector.h"
#include "view2d/brush/omChunksAndPts.hpp"

namespace OmBrushSelectUtils {
  om::common::SegIDSet FindSegIDsFromPoints(
      OmBrushOppInfo& info, om::pt3d_list_t& pts) {
    OmChunksAndPts chunksAndPts(info.segmentation, info.viewType);

    chunksAndPts.AddAllPtsThatIntersectVol(pts);

    return chunksAndPts.GetSegIDs();
  }

  std::shared_ptr<OmSegmentSelector> GetSelector(OmView2dState& state) {
    std::shared_ptr<OmSegmentSelector> selector = 
      state.getViewGroupState().GetOrCreateSelector(
          state.GetSegmentationID(), "Brush Select Line");
    selector->ShouldScroll(false);
    selector->AddToRecentList(false);
    selector->AutoCenter(false);
    return selector;
  }

  void AddOrSubtractSegments(OmSegmentSelector& selector,
      om::common::AddOrSubtract addOrSubtract, om::common::SegIDSet& segIDs) {
    if (om::common::AddOrSubtract::ADD == addOrSubtract) {
      selector.InsertSegments(segIDs);
    } else {
      selector.RemoveTheseSegments(segIDs);
    }
    selector.UpdateSelectionNow();
  }
};

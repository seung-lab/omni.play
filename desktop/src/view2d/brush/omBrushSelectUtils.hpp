#pragma once

#include "events/events.h"
#include "segment/omSegmentSelector.h"
#include "view2d/brush/omChunksAndPts.hpp"

class OmBrushSelectUtils {
 public:
  std::shared_ptr<om::common::SegIDSet> static FindSegIDsFromPoints(
      OmBrushOppInfo* info, om::pt3d_list_t* pts) {
    OmChunksAndPts chunksAndPts(info->segmentation, info->viewType);

    chunksAndPts.AddAllPtsThatIntersectVol(pts);

    return chunksAndPts.GetSegIDs();
  }

  void static SendEvent(OmBrushOppInfo* info, om::common::SegIDSet* segIDs) {
    OmSegmentSelector selector(info->segmentation->GetSDW(), NULL,
                               "view2d_selector");

    selector.ShouldScroll(false);
    selector.AddToRecentList(false);
    selector.AutoCenter(false);
    selector.AugmentListOnly(true);
    selector.AddOrSubtract(info->addOrSubract);

    if (om::common::AddOrSubtract::ADD == info->addOrSubract) {
      selector.InsertSegments(segIDs);

    } else {
      selector.RemoveSegments(segIDs);
    }

    selector.sendEvent();

    om::event::Redraw3d();
  }
};

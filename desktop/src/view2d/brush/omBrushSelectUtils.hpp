#pragma once

#include "events/omEvents.h"
#include "segment/omSegmentSelector.h"
#include "view2d/brush/omChunksAndPts.hpp"

class OmBrushSelectUtils {
 public:
  std::shared_ptr<
      std::unordered_set<om::common::SegID> > static FindSegIDsFromPoints(
      OmBrushOppInfo* info, om::pt3d_list_t* pts) {
    OmChunksAndPts chunksAndPts(info->segmentation, info->viewType);

    chunksAndPts.AddAllPtsThatIntersectVol(pts);

    return chunksAndPts.GetSegIDs();
  }

  void static SendEvent(OmBrushOppInfo* info,
                        std::unordered_set<om::common::SegID>* segIDs) {
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

    OmEvents::Redraw3d();
  }
};

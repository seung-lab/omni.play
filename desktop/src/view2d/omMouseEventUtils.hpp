#pragma once
#include "precomp.h"

#include "view2d/omView2dZoom.hpp"
#include "view2d/omMouseEventState.hpp"

class OmView2d;

class OmMouseEventUtils {
 public:
  static void PickToolAddToSelection(const SegmentDataWrapper& sdw,
                                     const om::coords::Global& dataClickPoint,
                                     OmView2d* v2d) {
    const om::common::SegID segID = sdw.GetVoxelValue(dataClickPoint);
    if (segID) {
      OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), v2d,
                            "view2dpick");
      sel.augmentSelectedSet(segID, true);
      sel.sendEvent();
    }
  }
};

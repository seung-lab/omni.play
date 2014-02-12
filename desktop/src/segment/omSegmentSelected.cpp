#include "segment/omSegment.h"
#include "utility/segmentDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegmentSelected.hpp"

void OmSegmentSelected::AugmentSelection(const SegmentDataWrapper& sdw) {
  OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), nullptr,
                        "OmSegmentSelected");
  sel.augmentSelectedSet(sdw.getID(), true);
  sel.sendEvent();
}

void OmSegmentSelected::ToggleValid() {
  if (!instance().sdw_.IsSegmentValid()) {
    return;
  }

  OmSegment* seg = instance().sdw_.GetSegment();

  if (seg->IsValidListType()) {
    OmActions::ValidateSegment(instance().sdw_,
                               om::common::SetValid::SET_NOT_VALID);
  } else {
    OmActions::ValidateSegment(instance().sdw_,
                               om::common::SetValid::SET_VALID);
  }

  om::event::SegmentModified();
}

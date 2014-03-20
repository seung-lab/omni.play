#pragma once
#include "precomp.h"

#include "common/common.h"
#include "common/enums.hpp"

class OmSegmentation;
class OmSegments;
class OmSelectSegmentsParams;
class SegmentationDataWrapper;
namespace om {
namespace segment {
class Selection;
}
}

class OmSegmentSelector {
 public:
  OmSegmentSelector(const SegmentationDataWrapper& sdw, void* sender,
                    const std::string& cmt);

  void selectJustThisSegment(const om::common::SegID segID,
                             const bool isSelected);
  void augmentSelectedSet(const om::common::SegID segID, const bool isSelected);

  void InsertSegments(const om::common::SegIDSet& segIDs);
  void RemoveSegments(const om::common::SegIDSet& segIDs);

  void selectJustThisSegment_toggle(const om::common::SegID segID);
  void augmentSelectedSet_toggle(const om::common::SegID segID);

  bool sendEvent();
  void selectNoSegments();

  void ShouldScroll(const bool shouldScroll);
  void AddToRecentList(const bool addToRecentList);
  void AutoCenter(const bool autoCenter);
  void AugmentListOnly(const bool augmentListOnly);
  void AddOrSubtract(const om::common::AddOrSubtract addSegments);

 private:
  OmSegments* segments_;
  om::segment::Selection* selection_;
  std::shared_ptr<OmSelectSegmentsParams> params_;

  void setSelectedSegment(const om::common::SegID segID);
};

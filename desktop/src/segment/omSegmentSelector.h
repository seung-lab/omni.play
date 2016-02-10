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
  ~OmSegmentSelector();

  // Disabling copy ctor, assign ctor so we don't send multiple events in destructor
  // Please use pointers to share this class
  OmSegmentSelector(const OmSegmentSelector& other) = delete;
  OmSegmentSelector& operator=(const OmSegmentSelector& other) = delete;

  void selectJustThisSegment(const om::common::SegID segID, const bool isSelected);
  void augmentSelectedSet(const om::common::SegID segID, const bool isSelected);

  void InsertSegments(const om::common::SegIDSet& segIDs);
  void InsertSegments(const om::common::SegIDList& segIDs);
  void RemoveSegments(const om::common::SegIDSet& segIDs);
  void RemoveTheseSegments(const om::common::SegIDSet& segIDs);

  void selectJustThisSegment_toggle(const om::common::SegID segID);
  void augmentSelectedSet_toggle(const om::common::SegID segID); 
  bool UpdateSelectionNow();
  // This should not be manually called, it should be called automatically during destruction
  bool sendEvent();
  bool IsSegmentSelected(const om::common::SegID segID);
  void selectNoSegments();

  void ShouldScroll(const bool shouldScroll);
  void AddToRecentList(const bool addToRecentList);
  void AutoCenter(const bool autoCenter);
  uint32_t GetOrderOfAdding(const om::common::SegID segID);

 private:
  void addSegmentToSelectionParameters(const om::common::SegID segID);
  void removeSegmentFromSelectionParameters(const om::common::SegID segID);

  OmSegments* segments_;
  om::segment::Selection* selection_;
  std::shared_ptr<OmSelectSegmentsParams> params_;
  void setSelectedSegment(const om::common::SegID segID);
  uint32_t numberOfAddedSegment;
};

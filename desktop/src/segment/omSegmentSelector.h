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

  void selectJustThisSegment(const om::common::SegID segID, const bool isSelected);
  void augmentSelectedSet(const om::common::SegID segID, const bool isSelected);

  void InsertSegments(const om::common::SegIDSet& segIDs);
  void InsertSegments(const om::common::SegIDList& segIDs);
  void RemoveSegments(const om::common::SegIDSet& segIDs);
  void RemoveTheseSegments(const om::common::SegIDSet& segIDs);

  void selectJustThisSegment_toggle(const om::common::SegID segID);
  void augmentSelectedSet_toggle(const om::common::SegID segID); 
  bool UpdateSelectionNow();
  bool sendEvent();
  bool IsSegmentSelected(const om::common::SegID segID);
  void selectNoSegments();

  void ShouldScroll(const bool shouldScroll);
  void AddToRecentList(const bool addToRecentList);
  void AutoCenter(const bool autoCenter);
  void AddOrSubtract(const om::common::AddOrSubtract addSegments);
  uint32_t GetOrderOfAdding(const om::common::SegID segID);

 private:
  void addSegmentToSelectionParameters(const om::common::SegID segID);
  void removeSegmentFromSelectionParameters(const om::common::SegID segID);
  common::SegID getNextInsertedSegment(om::common::SegIDMap segIDToOrders, uint32_t orderFrom);

  OmSegments* segments_;
  om::segment::Selection* selection_;
  std::shared_ptr<OmSelectSegmentsParams> params_;
  void setSelectedSegment(const om::common::SegID segID);
  uint32_t numberOfAddedSegment;

  std::unique_ptr<om::common::SegID> firstSelectedSegmentID_;
  std::unique_ptr<uint32_t> firstSelectedSegmentOrder_;
};

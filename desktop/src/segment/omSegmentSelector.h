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

  // prevent this segment from being selected
  void BlacklistAddSegment(const om::common::SegID);
  void BlacklistRemoveSegment(const om::common::SegID);
  bool IsBlacklistSegment(const om::common::SegID);

  void InsertSegments(const om::common::SegIDSet& segIDs);
  void InsertSegments(const om::common::SegIDList& segIDs);
  void RemoveSegments(const om::common::SegIDSet& segIDs);
  void RemoveSegments(const om::common::SegIDList& segIDs);


  void selectJustThisSegment_toggle(const om::common::SegID segID);
  void augmentSelectedSet_toggle(const om::common::SegID segID); 
  /*
   * Propagate changes into the selection. This  bypasses the action event and it also
   * triggers the event SegmentEvent::Modified (namely to kick off redrawing).
   * returns: true if there were any valid changes propagated into the selection.
   */
  bool UpdateSelectionNow();

  bool IsSegmentSelected(const om::common::SegID segID);
  void selectNoSegments();

  void ShouldScroll(const bool shouldScroll);
  void AddToRecentList(const bool addToRecentList);
  void AutoCenter(const bool autoCenter);
  uint32_t GetOrderOfAdding(const om::common::SegID segID);

  void SetFocusSegment(const om::common::SegID segID);
  om::common::SegID GetFocusSegment();

 private:
  void addSegmentToSelectionParameters(const om::common::SegID segID);
  void removeSegmentFromSelectionParameters(const om::common::SegID segID);

  /*
   * Call the segment selection action.  This should not be manually called.
   * It should be called automatically during destruction
   */
  bool sendEvent();

  OmSegments* segments_;
  om::segment::Selection* selection_;
  std::shared_ptr<OmSelectSegmentsParams> params_;
  om::common::SegIDSet blacklist_;
  uint32_t numberOfAddedSegment;
  uint32_t nextOrder_;
};

#include "actions/omActions.h"
#include "actions/omSelectSegmentParams.hpp"
#include "utility/set.hpp"
#include "project/omProject.h"
#include "segment/omSegments.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/selection.hpp"
#include "volume/omSegmentation.h"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegmentsImpl.h"

OmSegmentSelector::OmSegmentSelector(const SegmentationDataWrapper& sdw,
                                     void* sender, const std::string& cmt)
    : params_(std::make_shared<OmSelectSegmentsParams>()) {
  if (!sdw.IsSegmentationValid()) {
    throw om::ArgException(
        "Invalid SegmentationDataWrapper "
        "(OmSegmentSelector::OmSegmentSelector)");
  }

  segments_ = sdw.Segments();
  selection_ = &sdw.Segments()->Selection();
  nextOrder_ = selection_->GetNextOrder();

  params_->sdw = SegmentDataWrapper(sdw, 0);
  params_->sender = sender;
  params_->comment = cmt;
  params_->oldSelectedIDs = selection_->GetSelectedSegmentIDsWithOrder();
  params_->newSelectedIDs = params_->oldSelectedIDs;
  params_->autoCenter = false;
  params_->shouldScroll = true;
  params_->addToRecentList = true;
}

OmSegmentSelector::~OmSegmentSelector() {
  try {
    sendEvent();
  } catch (...) {
    log_errors << "Error in destructing Segment Selector. " <<
      "Event may not have been sent to complete selection action!" << std::endl;
  }
};


void OmSegmentSelector::selectNoSegments() {
    params_->newSelectedIDs.clear();
}

void OmSegmentSelector::selectJustThisSegment(
    const om::common::SegID segIDunknownLevel, const bool isSelected) {
  selectNoSegments();

  auto segID = segments_->FindRootID(segIDunknownLevel);
  if (!segID) {
    return;
  }

  // only select just one segment if we selected more than one previously?
  if (isSelected) {
    addSegmentToSelectionParameters(segID);
  }
}

void OmSegmentSelector::BlacklistSegment(const om::common::SegID segID) {
  blacklist_.insert(segID);
}

bool OmSegmentSelector::IsBlacklistSegment(const om::common::SegID segID) {
  return blacklist_.find(segID) != blacklist_.end();
}

// this indicates what segment we selected, also used to tell the segment
// list box to scroll to the selected row for this segment
void OmSegmentSelector::SetFocusSegment(const om::common::SegID segID) {
  params_->sdw.SetSegmentID(segID);
  OmSegmentSelected::Set(params_->sdw);
}

om::common::SegID OmSegmentSelector::GetFocusSegment() {
  return params_->sdw.GetSegmentID();
}

void OmSegmentSelector::InsertSegments(const om::common::SegIDSet& segIDs) {
  om::common::SegID rootID;
  for (auto id : segIDs) {
    rootID = segments_->FindRootID(id);
    addSegmentToSelectionParameters(rootID);
  }
}

void OmSegmentSelector::InsertSegments(const om::common::SegIDList& segIDs) {
  om::common::SegID rootID;
  for (auto id : segIDs) {
    rootID = segments_->FindRootID(id);
    addSegmentToSelectionParameters(rootID);
  }
}

void OmSegmentSelector::RemoveSegments(const om::common::SegIDList& segIDs) {
  om::common::SegID rootID;
  for (auto id : segIDs) {
    rootID = segments_->FindRootID(id);
    removeSegmentFromSelectionParameters(rootID);
  }
}

void OmSegmentSelector::RemoveSegments(const om::common::SegIDSet& segIDs) {
  om::common::SegID rootID;
  for (auto id : segIDs) {
    rootID = segments_->FindRootID(id);
    removeSegmentFromSelectionParameters(rootID);
  }
}

void OmSegmentSelector::augmentSelectedSet(
    const om::common::SegID segIDunknownLevel, const bool isSelected) {
  const om::common::SegID segID = segments_->FindRootID(segIDunknownLevel);

  if (!segID) {
    return;
  }

  if (isSelected) {
    addSegmentToSelectionParameters(segID);
  } else {
    removeSegmentFromSelectionParameters(segID);
  }
}

void OmSegmentSelector::selectJustThisSegment_toggle(
    const om::common::SegID segIDunknownLevel) {
  const om::common::SegID segID = segments_->FindRootID(segIDunknownLevel);
  if (!segID) {
    return;
  }

  const bool isSelected = selection_->IsSegmentSelected(segID);
  selectJustThisSegment(segID, !isSelected);
}

void OmSegmentSelector::augmentSelectedSet_toggle(
    const om::common::SegID segIDunknownLevel) {
  const om::common::SegID segID = segments_->FindRootID(segIDunknownLevel);
  if (!segID) {
    return;
  }

  const bool isSelected = selection_->IsSegmentSelected(segID);
  augmentSelectedSet(segID, !isSelected);
}

bool OmSegmentSelector::IsSegmentSelected(const om::common::SegID segID) {
  return selection_->IsSegmentSelected(segID);
}

bool OmSegmentSelector::UpdateSelectionNow() {
  // add any newly selected ids to the master selection list
  bool selectionIsChanged = 
    selection_->UpdateSegmentSelection(params_->newSelectedIDs, params_->addToRecentList);

  if (selectionIsChanged) {
    // note the orders may be modified after update selection
    params_->newSelectedIDs = selection_->GetSelectedSegmentIDsWithOrder();
    nextOrder_ = selection_->GetNextOrder();

    OmCacheManager::TouchFreshness();
    om::event::SegmentModified(params_);
  }

  return selectionIsChanged;
}

bool OmSegmentSelector::sendEvent() {
  if (params_->oldSelectedIDs == params_->newSelectedIDs) {
    // no change in selected set
    return false;
  }

  // log_debugs(segmentSelector) << params_->oldSelectedIDs << "\n";

  OmActions::SelectSegments(params_);

  return true;
}

void OmSegmentSelector::ShouldScroll(const bool shouldScroll) {
  params_->shouldScroll = shouldScroll;
}

void OmSegmentSelector::AddToRecentList(const bool addToRecentList) {
  params_->addToRecentList = addToRecentList;
}

void OmSegmentSelector::AutoCenter(const bool autoCenter) {
  params_->autoCenter = autoCenter;
}

void OmSegmentSelector::addSegmentToSelectionParameters(om::common::SegID segID) {
  params_->newSelectedIDs.insert(std::pair<om::common::SegID, uint32_t>(segID, nextOrder_++));
  SetFocusSegment(segID);
}

void OmSegmentSelector::removeSegmentFromSelectionParameters(om::common::SegID segID) {
  params_->newSelectedIDs.erase(segID);
  // only update the selected segment if any was selected
  if (params_->newSelectedIDs.begin() != params_->newSelectedIDs.end()) {
    SetFocusSegment(params_->newSelectedIDs.begin()->first);
  }
}

uint32_t OmSegmentSelector::GetOrderOfAdding(const om::common::SegID segID) {
  auto iter = params_->newSelectedIDs.find(segID);

  if (iter != params_->newSelectedIDs.end()) {
    return iter->second;
  } else {
    return 0;
  }
}


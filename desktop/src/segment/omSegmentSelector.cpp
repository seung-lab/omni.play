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

  params_->sdw = SegmentDataWrapper(sdw, 0);
  params_->sender = sender;
  params_->comment = cmt;
  params_->oldSelectedIDs = selection_->GetSelectedSegmentIDsWithOrder();
  params_->newSelectedIDs = params_->oldSelectedIDs;
  params_->autoCenter = false;
  params_->shouldScroll = true;
  params_->addToRecentList = true;

  params_->augmentListOnly = false;
  params_->addOrSubtract = om::common::AddOrSubtract::ADD;
}

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

  if (params_->oldSelectedIDs.size() > 1 || isSelected) {
    addSegmentToSelectionParameters(segID);
  }

  // TODO::augmentAlwaysSelect wtf? why does this always select during augment?!
  //setSelectedSegment(segID);
}

void OmSegmentSelector::setSelectedSegment(const om::common::SegID segID) {
  params_->sdw.SetSegmentID(segID);
  OmSegmentSelected::Set(params_->sdw);
}

void OmSegmentSelector::InsertSegments(const om::common::SegIDSet& segIDs) {
  om::common::SegID rootID;
  for (auto id : segIDs) {
    rootID = segments_->FindRootID(id);
    addSegmentToSelectionParameters(rootID);
  }
}

void OmSegmentSelector::InsertSegmentsOrdered(const om::common::SegIDList& segIDs) {
  om::common::SegID rootID;
  for (auto id : segIDs) {
    rootID = segments_->FindRootID(id);
    addSegmentToSelectionParameters(rootID);
  }
}

/* this actually removes all the segments and inserts the parameter segments. huh? poor naming choice*/
void OmSegmentSelector::RemoveSegments(const om::common::SegIDSet& segIDs) {
  selectNoSegments();

  om::common::SegID rootID;

  for (auto id : segIDs) {
    rootID = segments_->FindRootID(id);
    addSegmentToSelectionParameters(rootID);
  }
}

/* this function actually removes the input segments*/
void OmSegmentSelector::RemoveTheseSegments(const om::common::SegIDSet& segIDs)
{
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

  setSelectedSegment(segID);
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

bool OmSegmentSelector::sendEvent() {
  om::common::SegIDSet oldSelectedIDs;
  boost::copy(params_->oldSelectedIDs | boost::adaptors::map_keys,
                std::inserter(oldSelectedIDs, oldSelectedIDs.begin()));
  om::common::SegIDSet newSelectedIDs;
  boost::copy(params_->newSelectedIDs | boost::adaptors::map_keys,
                std::inserter(newSelectedIDs, newSelectedIDs.begin()));
  if (params_->augmentListOnly) {
    if (om::common::AddOrSubtract::ADD == params_->addOrSubtract) {
      if (om::set::SetAContainsB(oldSelectedIDs,
                                 newSelectedIDs)) {
        // already added
        return false;
      }
    } else {
      if (om::set::SetsAreDisjoint(oldSelectedIDs,
                                   newSelectedIDs)) {
        // no segments to be removed are selected
        return false;
      }
    }

  } else {
    if (oldSelectedIDs == newSelectedIDs) {
      // no change in selected set
      return false;
    }
  }

  // log_debugs(segmentSelector) << params_->oldSelectedIDs << "\n";

  // TODO::saveableEventOnly only usage in brushselect, move this code into actionimpl+ redraw!
  if (params_->augmentListOnly) {
    // disable undo option for now
    om::segment::Selection* selection = params_->sdw.Selection();

    if (om::common::AddOrSubtract::ADD == params_->addOrSubtract) {
      selection->AddToSegmentSelection(params_->newSelectedIDs);
    } else {
      selection->RemoveFromSegmentSelection(params_->newSelectedIDs);
    }
  } else {
    OmActions::SelectSegments(params_);
  }

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

void OmSegmentSelector::AugmentListOnly(const bool augmentListOnly) {
  params_->augmentListOnly = augmentListOnly;
}

void OmSegmentSelector::AddOrSubtract(
    const om::common::AddOrSubtract addOrSubtract) {
  params_->addOrSubtract = addOrSubtract;
}

void OmSegmentSelector::addSegmentToSelectionParameters(om::common::SegID segID) {
  uint32_t newOrder =  params_->newSelectedIDs.size() + 1;
  params_->newSelectedIDs.insert(std::pair<om::common::SegID, uint32_t>(segID, newOrder));
}

void OmSegmentSelector::removeSegmentFromSelectionParameters(om::common::SegID segID) {
  params_->newSelectedIDs.erase(segID);
}

uint32_t OmSegmentSelector::GetOrderOfAdding(const om::common::SegID segID) {
  return selection_->GetOrderOfAdding(segID);
}


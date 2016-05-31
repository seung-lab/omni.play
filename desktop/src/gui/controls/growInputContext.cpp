#include "precomp.h"
#include "gui/controls/growInputContext.hpp"
#include "gui/controls/inputContext.hpp"
#include "gui/controls/viewInputContext.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/growing.hpp"
#include "utility/dataWrappers.h"
#include "segment/actions/omJoinSplitRunner.hpp"
#include "segment/omSegments.h"
#include "segment/omSegmentSelector.h"
#include "events/events.h"
#include "users/omUsers.h"
#include "segment/types.hpp"

/*
 ********************** CONSTANTS **************************
 */
const double GrowInputContext::THRESHOLD_STEP = .001;
const double GrowInputContext::MAX_THRESHOLD = 1;
const double GrowInputContext::MIN_THRESHOLD = 0;

/*
 ********************** PUBLIC METHODS **************************
 */
GrowInputContext::GrowInputContext(ViewInputConversion* viewInputConversion,
      OmViewGroupState* viewGroupState, om::tool::mode tool)
  : ViewInputContext(viewInputConversion, viewGroupState), tool_(tool) {
}

bool GrowInputContext::mousePressEvent(QMouseEvent* mouseEvent) {
  Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
  Qt::MouseButton button = mouseEvent->button();
  switch ((int)button | (int)modifiers) {
    case (int)Qt::MiddleButton:
    case (int)Qt::MiddleButton | (int)Qt::ShiftModifier
      | (int)Qt::AltModifier:
    case (int)Qt::MiddleButton | (int)Qt::ShiftModifier:
      return GrowCoordinates(mouseEvent->x(), mouseEvent->y());
    case (int)Qt::RightButton | (int)Qt::ShiftModifier:
      // trim at segment but keep segment and blacklist around it
      return TrimAndBlacklistAdjacentCoordinates(mouseEvent->x(),
          mouseEvent->y());
    case (int)Qt::RightButton | (int)Qt::ShiftModifier
      | (int)Qt::AltModifier:
      // trim at segment but keep segment
      return TrimAdjacentCoordinates(mouseEvent->x(), mouseEvent->y());
    default:
      return false;
  }
}

bool GrowInputContext::mouseReleaseEvent(QMouseEvent* mouseEvent) {
  Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
  Qt::MouseButton button = mouseEvent->button();
  switch ((int)button | (int)modifiers) {
    case (int)Qt::MiddleButton:
    case (int)Qt::MiddleButton | (int)Qt::AltModifier:
      // Mouse release does not need to take over controls
      viewGroupState_->EndSelector();
      return false;
    default:
      return false;
  }
}

bool GrowInputContext::mouseDoubleClickEvent(QMouseEvent* mouseEvent) {
  Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
  Qt::MouseButton button = mouseEvent->button();
  switch ((int)button | (int)modifiers) {
    case (int)Qt::MiddleButton | (int)Qt::ShiftModifier:
      // grow and clear blacklist
      return GrowCoordinatesAndRemoveAdjacentBlacklist(mouseEvent->x(),
          mouseEvent->y());
    case (int)Qt::RightButton | (int)Qt::ShiftModifier:
      // trim and remove segment completely and blacklist segment
      return TrimAndBlacklistCoordinates(mouseEvent->x(), mouseEvent->y());
    case (int)Qt::RightButton | (int)Qt::ShiftModifier
      | (int)Qt::AltModifier:
      // trim and remove segment completely
      return TrimAndRemoveCoordinates(mouseEvent->x(), mouseEvent->y());
    default:
      return false;
  }
}

bool GrowInputContext::wheelEvent(QWheelEvent *wheelEvent) {
  Qt::KeyboardModifiers modifiers = wheelEvent->modifiers();
  const int numDegrees = wheelEvent->delta() / 8;
  const int numSteps = numDegrees / 15;
  switch ((int)modifiers) {
    case (int)Qt::ShiftModifier | (int)Qt::AltModifier:
      return GrowIncremental(numSteps >= 0);
    default:
      return false;
  }
}

bool GrowInputContext::keyReleaseEvent(QKeyEvent *keyEvent) {
  Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
  int key = keyEvent->key();
  switch (key | (int) modifiers) {
    case (int)Qt::Key_Shift:
    case (int)Qt::Key_Shift | (int)Qt::AltModifier:
      viewGroupState_->EndSelector();
      return false;
    default:
      return false;
  }
}

bool GrowInputContext::GrowCoordinates(int x, int y) {
  log_infos << "Grow coordinates";
  return growHelper(x, y, std::function<void(OmSegmentSelector&,
        om::common::SegID)>());
}

bool GrowInputContext::GrowCoordinatesAndRemoveAdjacentBlacklist(int x, int y) {
  log_infos << "Grow coordinates and remove adjacent blacklist";
  return growHelper(x, y, [=] (OmSegmentSelector& selector,
        om::common::SegID selectedID) {
      blacklistRemoveAdjacent(selector, selectedID);
      });
}

bool GrowInputContext::TrimAdjacentCoordinates(int x, int y) {
  log_infos << "Trim adjacent segments";
  return trimHelper(x, y, std::function<void(OmSegmentSelector&,
        om::common::SegID)>());
}

bool GrowInputContext::TrimAndRemoveCoordinates(int x, int y) {
  log_infos << "Trim and remove selected segment";
  return trimHelper(x, y, [=] (OmSegmentSelector& selector,
        om::common::SegID selectedID) {
      selector.augmentSelectedSet(selectedID, false);
      });
}

bool GrowInputContext::TrimAndBlacklistAdjacentCoordinates(int x, int y) {
  log_infos << "Trim and blacklist adjacent segments";
  return trimHelper(x, y, [=] (OmSegmentSelector& selector,
        om::common::SegID selectedID) {
      blacklistAddAdjacent(selector, selectedID);
      });
}

bool GrowInputContext::TrimAndBlacklistCoordinates(int x, int y) {
  log_infos << "Trim and blacklist selected segment";
  return trimHelper(x, y, [=] (OmSegmentSelector& selector,
        om::common::SegID selectedID) {
      selector.BlacklistAddSegment(selectedID);
      selector.augmentSelectedSet(selectedID, false);
      });
}

bool GrowInputContext::GrowIncremental(bool isGrowing) {
  log_infos << "Grow Incremental";
  std::shared_ptr<OmSegmentSelector> selector =
    viewGroupState_->GetOrCreateSelector(
        viewGroupState_->Segmentation().GetSegmentationID(), "Grow Selector");

  if (!selector->GetFocusSegment()) {
    log_infos << "No segment selected";
    return false;
  }

  om::common::SegIDList trimSeedIDs = grow(selector->GetFocusSegment(),
      updateThreshold(isGrowing), *selector);
  trim(*selector, trimSeedIDs);

  selector->RemoveSegments(trimSeedIDs);

  selector->UpdateSelectionNow();
  return true;
}


/*
 ********************** PRIVATE METHODS **************************
 */
bool GrowInputContext::growHelper(int x, int y, std::function<
    void(OmSegmentSelector&, om::common::SegID)> preGrowFunction) {
  std::shared_ptr<OmSegmentSelector> selector;
  boost::optional<SegmentDataWrapper> segmentDataWrapper;
  std::tie(selector, segmentDataWrapper) = getSelection(x, y);

  if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
    log_infos << "No segment selected";
    return false;
  }

  om::common::SegID selectedID = segmentDataWrapper->GetSegmentID();

  if (preGrowFunction) {
    preGrowFunction(*selector, selectedID);
  }

  grow(selectedID, OmProject::Globals().Users().UserSettings()
      .getGrowThreshold(), *selector);

  selector->SetFocusSegment(selectedID);
  selector->UpdateSelectionNow();
  return true;
}

bool GrowInputContext::trimHelper(int x, int y, std::function<
    void(OmSegmentSelector&, om::common::SegID)> postTrimFunction) {
  std::shared_ptr<OmSegmentSelector> selector;
  boost::optional<SegmentDataWrapper> segmentDataWrapper;
  std::tie(selector, segmentDataWrapper) = getSelection(x, y);

  if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
    log_infos << "No segment selected";
    return false;
  }

  om::common::SegID selectedID = segmentDataWrapper->GetSegmentID();

  trim(*selector, selectedID);

  if (postTrimFunction) {
    postTrimFunction(*selector, selectedID);
  }

  selector->UpdateSelectionNow();
  return true;
}

std::tuple<std::shared_ptr<OmSegmentSelector>,
  boost::optional<SegmentDataWrapper>> GrowInputContext::getSelection(
      int x, int y) {
  boost::optional<SegmentDataWrapper> segmentDataWrapper;
  std::shared_ptr<OmSegmentSelector> selector;

  segmentDataWrapper = viewInputConversion_->FindSegment(x, y);

  selector =
    viewGroupState_->GetOrCreateSelector(viewGroupState_->Segmentation()
        .GetSegmentationID(), "Grow Selector");

  return std::make_tuple(selector, segmentDataWrapper);
}


std::tuple<om::common::SegIDList, om::common::SegIDList>
  GrowInputContext::findNotSelected(om::common::SegID selectedID,
      double threshold, OmSegmentSelector& selector) {
  return viewGroupState_->GetGrowing()->FindNotSelected(
      selectedID, threshold, selector,
      viewGroupState_->Segmentation().Segments()->GetAdjacencyMap());
}

om::common::SegIDList GrowInputContext::findSelected(om::common::SegIDList seedIDs,
    OmSegmentSelector& selector) {
  return viewGroupState_->GetGrowing()
    ->FindSelected(seedIDs, selector,
      viewGroupState_->Segmentation().Segments()->GetAdjacencyMap());
}

om::common::SegIDList GrowInputContext::grow(om::common::SegID seedID,
    double threshold, OmSegmentSelector& selector) {
  om::common::SegIDList growIDs, trimSeedIDs;
  std::tie(growIDs, trimSeedIDs) = findNotSelected(seedID, threshold, selector);

  selector.InsertSegments(growIDs);
  selector.SetFocusSegment(seedID);
  return trimSeedIDs;
}

void GrowInputContext::trim(OmSegmentSelector& selector, om::common::SegID seedID) {
  om::common::SegIDList seedIDs;
  seedIDs.push_back(seedID);
  trim(selector, seedIDs);
}

void GrowInputContext::trim(OmSegmentSelector& selector, om::common::SegIDList seedIDs) {
  om::common::SegIDList removeIDs = findSelected(seedIDs, selector);
  selector.RemoveSegments(removeIDs);
}

double GrowInputContext::updateThreshold(bool isGrowing) {
  double threshold = OmProject::Globals().Users().UserSettings()
    .getGrowThreshold();

  if (isGrowing) {
    threshold += THRESHOLD_STEP;
  } else {
    threshold -= THRESHOLD_STEP;
  }

  // enforce limits to the threshold
  if (threshold > MAX_THRESHOLD) {
    threshold = MAX_THRESHOLD;
  }

  if (threshold < MIN_THRESHOLD) {
    threshold = MIN_THRESHOLD;
  }

  OmProject::Globals().Users().UserSettings().setGrowThreshold(threshold);
  om::event::UserSettingsUpdated();
  return threshold;
}

void GrowInputContext::blacklistRemoveAdjacent(OmSegmentSelector& selector,
    om::common::SegID seedID) {
  auto adjacencyMap = viewGroupState_->Segmentation().Segments()
    ->GetAdjacencyMap();
  auto iter = adjacencyMap.find(seedID);
  if (iter != adjacencyMap.end()) {
    for (auto edge : iter->second) {
      selector.BlacklistRemoveSegment(edge->otherNodeID(seedID));
    }
  }
}

void GrowInputContext::blacklistAddAdjacent(OmSegmentSelector& selector,
    om::common::SegID seedID) {
  om::common::SegIDList growIDs, trimSeedIDs;
  uint32_t currOrderOfAdding = selector.GetOrderOfAdding(seedID);

  auto adjacencyMap = viewGroupState_->Segmentation().Segments()
    ->GetAdjacencyMap();
  auto iter = adjacencyMap.find(seedID);
  if (iter != adjacencyMap.end()) {
    for (auto edge : iter->second) {
      uint32_t nextOrderOfAdding = selector.GetOrderOfAdding(
          edge->otherNodeID(seedID));
      if (!nextOrderOfAdding || nextOrderOfAdding > currOrderOfAdding) {
        selector.BlacklistAddSegment(edge->otherNodeID(seedID));
      }
    }
  }
}


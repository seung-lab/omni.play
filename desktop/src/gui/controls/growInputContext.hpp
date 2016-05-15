#pragma once

#include "precomp.h"
#include "gui/controls/inputContext.hpp"
#include "gui/controls/viewStateInputContext.hpp"
#include "gui/controls/findSegment.hpp"
#include "gui/controls/findGlobalCoordinates.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/growing.hpp"
#include "utility/dataWrappers.h"
#include "segment/actions/omJoinSplitRunner.hpp"
#include "segment/omSegments.h"
#include "segment/omSegmentSelector.h"
#include "events/events.h"
#include "users/omUsers.h"
#include "segment/types.hpp"

class GrowInputContext
: public InputContext,
  public ViewStateInputContext,
  public FindSegment {
 public:
  GrowInputContext(OmViewGroupState* viewGroupState,
      om::tool::mode tool,
      std::function<boost::optional<SegmentDataWrapper>(int, int)>
        findSegmentFunction)
    : ViewStateInputContext(viewGroupState), tool_(tool),
      FindSegment(findSegmentFunction) {
    }

  virtual bool mousePressEvent(QMouseEvent* mouseEvent) override {
    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
    Qt::MouseButton button = mouseEvent->button();
    switch ((int)button | (int)modifiers) {
      case (int)Qt::LeftButton:
      case (int)Qt::LeftButton | (int)Qt::ShiftModifier:
        return growCoordinates(mouseEvent->x(), mouseEvent->y());
      case (int)Qt::RightButton | (int)Qt::ShiftModifier:
        // trim at segment but keep segment and blacklist around it
        return trimAndBlacklistAdjacentCoordinates(mouseEvent->x(),
            mouseEvent->y());
      case (int)Qt::RightButton | (int)Qt::ShiftModifier
        | (int)Qt::ControlModifier:
        // trim at segment but keep segment
        return trimAdjacentCoordinates(mouseEvent->x(), mouseEvent->y());
      default:
        return false;
    }
  }

  virtual bool mouseReleaseEvent(QMouseEvent* mouseEvent) override {
    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
    Qt::MouseButton button = mouseEvent->button();
    switch ((int)button | (int)modifiers) {
      case (int)Qt::LeftButton:
      case (int)Qt::LeftButton | (int)Qt::ControlModifier:
        viewGroupState_->EndSelector();
        return true;
      default:
        return false;
    }
  }

  virtual bool mouseDoubleClickEvent(QMouseEvent* mouseEvent) override {
    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
    Qt::MouseButton button = mouseEvent->button();
    switch ((int)button | (int)modifiers) {
      case (int)Qt::RightButton | (int)Qt::ShiftModifier:
        // trim and remove segment completely and blacklist segment
        return trimAndBlacklistCoordinates(mouseEvent->x(), mouseEvent->y());
      case (int)Qt::RightButton | (int)Qt::ShiftModifier
        | (int)Qt::ControlModifier:
        // trim and remove segment completely
        return trimAndRemoveCoordinates(mouseEvent->x(), mouseEvent->y());
      default:
        return false;
    }
  }

  virtual bool wheelEvent(QWheelEvent *wheelEvent) {
    Qt::KeyboardModifiers modifiers = wheelEvent->modifiers();
    const int numDegrees = wheelEvent->delta() / 8;
    const int numSteps = numDegrees / 15;
    switch ((int)modifiers) {
      case (int)Qt::ShiftModifier | (int)Qt::ControlModifier:
        return growIncremental(numSteps >= 0);
      default:
        return false;
    }
  }

  virtual bool keyReleaseEvent(QKeyEvent *keyEvent) override {
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
    int key = keyEvent->key();
    switch (key | (int) modifiers) {
      case (int)Qt::Key_Shift:
      case (int)Qt::Key_Shift | (int)Qt::ControlModifier:
        viewGroupState_->EndSelector();
      default:
        return false;
    }
  }

  const double THRESHOLD_STEP = .001;
  const double MAX_THRESHOLD = 1;
  const double MIN_THRESHOLD = 0;

 private:
  om::tool::mode tool_;

  double getUpdatedThreshold(bool isGrowing) {
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

  bool growCoordinates(int x, int y) {
    std::cout << "Grow coordinates"<<std::endl;

    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      log_infos << "No segment selected";
      return false;
    }

    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          viewGroupState_->Segmentation().GetSegmentationID(), "Grow Selector");

    selector->SetFocusSegment(segmentDataWrapper->GetSegmentID());
    grow(*selector, segmentDataWrapper->GetSegmentID(),
        OmProject::Globals().Users().UserSettings().getGrowThreshold());

    selector->UpdateSelectionNow();
    return true;
  }

  bool trimAdjacentCoordinates(int x, int y) {
    log_infos << "Trim adjacent segments";
    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      log_infos << "No segment selected";
      return false;
    }

    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          viewGroupState_->Segmentation().GetSegmentationID(), "Grow Selector");

    trim(*selector, segmentDataWrapper->GetSegmentID());

    selector->UpdateSelectionNow();
    return true;
  }

  bool trimAndRemoveCoordinates(int x, int y) {
    log_infos << "Trim and remove selected segment";
    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      log_infos << "No segment selected";
      return false;
    }

    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          viewGroupState_->Segmentation().GetSegmentationID(), "Grow Selector");

    trim(*selector, segmentDataWrapper->GetSegmentID());
    selector->augmentSelectedSet(segmentDataWrapper->GetSegmentID(),
        false);

    selector->UpdateSelectionNow();
    return true;
  }

  void blacklistAdjacent(OmSegmentSelector& selector,
      om::common::SegID seedID) {
    auto adjacencyMap = viewGroupState_->Segmentation().Segments()
      ->GetAdjacencyMap();
    auto iter = adjacencyMap.find(seedID);
    if (iter != adjacencyMap.end()) {
      for (auto edge : iter->second) {
        selector.BlacklistSegment(edge->otherNodeID(seedID));
      }
    }
  }

  bool trimAndBlacklistAdjacentCoordinates(int x, int y) {
    log_infos << "trim and blacklist adjacent segments";
    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      log_infos << "No segment selected";
      return false;
    }

    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          viewGroupState_->Segmentation().GetSegmentationID(), "Grow Selector");

    trim(*selector, segmentDataWrapper->GetSegmentID());
    blacklistAdjacent(*selector, segmentDataWrapper->GetSegmentID());

    selector->UpdateSelectionNow();
    return true;
  }

  bool trimAndBlacklistCoordinates(int x, int y) {
    log_infos << "trim and blacklist selected segment";
    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      log_infos << "No segment selected";
      return false;
    }

    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          viewGroupState_->Segmentation().GetSegmentationID(), "Grow Selector");

    trim(*selector, segmentDataWrapper->GetSegmentID());
    selector->BlacklistSegment(segmentDataWrapper->GetSegmentID());
    selector->augmentSelectedSet(segmentDataWrapper->GetSegmentID(), false);

    selector->UpdateSelectionNow();
    return true;
  }

  bool growIncremental(bool isGrowing) {
    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          viewGroupState_->Segmentation().GetSegmentationID(), "Grow Selector");
    if (!selector->GetFocusSegment()) {
      return false;
    }

    om::common::SegIDList trimSeedIDs = grow(*selector,
        selector->GetFocusSegment(),
        getUpdatedThreshold(isGrowing));
    trim(*selector, trimSeedIDs);

    selector->RemoveSegments(trimSeedIDs);

    selector->UpdateSelectionNow();
    return true;
  }

  std::shared_ptr<OmSegmentSelector> getSelector(int x, int y) {
    std::shared_ptr<OmSegmentSelector> selector;

    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      log_infos << "No segment selected";
      return selector;
    }


    selector = viewGroupState_->GetOrCreateSelector(
          viewGroupState_->Segmentation().GetSegmentationID(), "Grow Selector");
    selector->SetFocusSegment(segmentDataWrapper->GetSegmentID());

    return selector;
  }


  std::tuple<om::common::SegIDList, om::common::SegIDList>
    FindNotSelected(OmSegmentSelector& selector, double threshold) {
    return viewGroupState_->GetGrowing()->FindNotSelected(
        selector.GetFocusSegment(), threshold,
        viewGroupState_->Segmentation().Segments()->GetAdjacencyMap(),
        selector);
  }

  om::common::SegIDList FindSelected(OmSegmentSelector& selector,
      om::common::SegIDList seedIDs) {
    return viewGroupState_->GetGrowing()
      ->FindSelected(seedIDs, selector,
        viewGroupState_->Segmentation().Segments()->GetAdjacencyMap());
  }

  om::common::SegIDList grow(OmSegmentSelector& selector,
     om::common::SegID seedID, double threshold) {
    om::common::SegID selectedID = seedID;
    om::common::SegIDList growIDs, trimSeedIDs;
    std::tie(growIDs, trimSeedIDs) = FindNotSelected(selector, threshold);

    selector.InsertSegments(growIDs);
    selector.SetFocusSegment(selectedID);
    return trimSeedIDs;
  }

  void trim(OmSegmentSelector& selector, om::common::SegIDList seedIDs) {
    om::common::SegIDList removeIDs = FindSelected(selector, seedIDs);
    selector.RemoveSegments(removeIDs);
  }

  void trim(OmSegmentSelector& selector, om::common::SegID seedID) {
    om::common::SegIDList seedIDs;
    seedIDs.push_back(seedID);
    trim(selector, seedIDs);
  }

};

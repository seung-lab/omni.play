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
#include <limits>

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
        return pruneAndBlacklistAdjacentCoordinates(mouseEvent->x(),
            mouseEvent->y());
      case (int)Qt::RightButton | (int)Qt::ShiftModifier
        | (int)Qt::ControlModifier:
        // trim at segment but keep segment
        return pruneAdjacentCoordinates(mouseEvent->x(), mouseEvent->y());
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
        return pruneAndBlacklistCoordinates(mouseEvent->x(), mouseEvent->y());
      case (int)Qt::RightButton | (int)Qt::ShiftModifier
        | (int)Qt::ControlModifier:
        // trim and remove segment completely
        return pruneAndRemoveCoordinates(mouseEvent->x(), mouseEvent->y());
      default:
        return false;
    }
  }

  virtual bool wheelEvent(QWheelEvent *wheelEvent) {
    Qt::KeyboardModifiers modifiers = wheelEvent->modifiers();
    const int numDegrees = wheelEvent->delta() / 8;
    const int numSteps = numDegrees / 15;
    switch ((int)modifiers) {
      case (int)Qt::ShiftModifier & (int)Qt::ControlModifier:
        return growIncremental(numSteps >= 0);
      default:
        return false;
    }
  }

  virtual bool keyReleaseEvent(QKeyEvent *keyEvent) override {
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
    int key = keyEvent->key();
    switch (key | (int) modifiers) {
      case Qt::Key_Shift:
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
    return pruneCoordinatesToThreshold(x, y, OmProject::Globals().Users()
        .UserSettings().getGrowThreshold(), false, false, false);
  }

  bool pruneAdjacentCoordinates(int x, int y) {
    return pruneCoordinatesToThreshold(x, y, MAX_THRESHOLD, true, false, false);
  }

  bool pruneAndRemoveCoordinates(int x, int y) {
    return pruneCoordinatesToThreshold(x, y, MAX_THRESHOLD, true, true, false);
  }

  bool pruneAndBlacklistAdjacentCoordinates(int x, int y) {
    return pruneCoordinatesToThreshold(x, y, MAX_THRESHOLD, true, false, true);
  }

  bool pruneAndBlacklistCoordinates(int x, int y) {
    return pruneCoordinatesToThreshold(x, y, MAX_THRESHOLD, true, true, true);
  }

  bool growIncremental(bool isGrowing) {
    return pruneToThreshold(getUpdatedThreshold(isGrowing),
        boost::optional<om::common::SegID>(), true, false, false);
  }

  /*
   * Takes coordinates and transforms to segmentid if necessary
   */
  bool pruneCoordinatesToThreshold(int x, int y, double threshold,
      bool shouldPruneToThreshold, bool shouldRemoveSelectedSegment,
      bool shouldAddToBlacklist) {
    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      log_infos << "No segment selected";
      return false;
    }

    return pruneToThreshold(threshold, boost::optional<om::common::SegID>(
          segmentDataWrapper->GetSegmentID()), shouldPruneToThreshold,
        shouldRemoveSelectedSegment, shouldAddToBlacklist);
  }

  bool pruneToThreshold(double threshold,
      boost::optional<om::common::SegID> segID, bool shouldPruneToThreshold,
      bool shouldKeepSelectedSegment, bool shouldAddToBlacklist) {
    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          viewGroupState_->Segmentation().GetSegmentationID(), "Grow Selector");

    if (segID) {
      selector->SetFocusSegment(*segID);
    }

    return pruneFocusToThreshold(*selector, threshold, shouldPruneToThreshold,
        shouldKeepSelectedSegment, shouldAddToBlacklist);
  }

  bool pruneFocusToThreshold(OmSegmentSelector& selector, double threshold,
      bool shouldPruneToThreshold, bool shouldKeepSelectedSegment,
      bool shouldAddToBlacklist) {
    const std::unordered_map<om::common::SegID,
      std::vector <om::segment::Edge*>>& adjacencyMap = viewGroupState_
        ->Segmentation().Segments()->GetAdjacencyMap();

    om::common::SegID focusSegment = selector.GetFocusSegment();

    om::common::SegIDList vecToAdd, vecToRemove;
    std::tie(vecToAdd, vecToRemove) =
      viewGroupState_->GetGrowing()->PruneBreadthFirstSearch(selector, threshold,
        adjacencyMap);

    selector.InsertSegments(vecToAdd);

    if (shouldPruneToThreshold) {
      selector.RemoveSegments(vecToRemove);
    }

    if (!shouldKeepSelectedSegment) {
      selector.augmentSelectedSet(focusSegment, false);
    }

    if (shouldAddToBlacklist) {
      if (shouldKeepSelectedSegment) {
        auto iter = adjacencyMap.find(focusSegment);
        if (iter != adjacencyMap.end()) {
          for (auto edge : iter->second) {
            selector.BlacklistSegment(edge.otherNodeID(focusSegment);
          }
        }
      } else {
        selector.BlacklistSegment(focusSegment);
      }
    }

    // reset the focus element to the first element that was clicked on
    selector.SetFocusSegment(focusSegment);
    selector.UpdateSelectionNow();
    return true;
  }


};

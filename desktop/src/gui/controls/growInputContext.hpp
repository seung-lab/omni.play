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
      case (int)Qt::LeftButton | (int)Qt::ControlModifier:
      case (int)Qt::LeftButton | (int)Qt::ControlModifier
        | (int)Qt::ShiftModifier:
        return trimCoordinates(mouseEvent->x(), mouseEvent->y());
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

  virtual bool wheelEvent(QWheelEvent *wheelEvent) {
    Qt::KeyboardModifiers modifiers = wheelEvent->modifiers();
    const int numDegrees = wheelEvent->delta() / 8;
    const int numSteps = numDegrees / 15;
    switch ((int)modifiers) {
      case (int)Qt::ShiftModifier:
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
    return growCoordinatesToThreshold(x, y, OmProject::Globals().Users()
        .UserSettings().getGrowThreshold());
  }

  bool trimCoordinates(int x, int y) {
    return growCoordinatesToThreshold(x, y, MAX_THRESHOLD);
  }

  bool growIncremental(bool isGrowing) {
    return growToThreshold(getUpdatedThreshold(isGrowing),
        boost::optional<om::common::SegID>());
  }

  bool growCoordinatesToThreshold(int x, int y, double threshold) {
    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      log_infos << "No segment selected";
      return false;
    }

    return growToThreshold(threshold, boost::optional<om::common::SegID>(
          segmentDataWrapper->GetSegmentID()));
  }

  bool growToThreshold(double threshold,
      boost::optional<om::common::SegID> segID) {
    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          viewGroupState_->Segmentation().GetSegmentationID(), "Grow Selector");
    if (segID) {
      selector->SetFocusSegment(*segID);
    }
    return growToThreshold(*selector, threshold);
  }

  bool growToThreshold(OmSegmentSelector& selector, double threshold) {
    om::common::SegID focusSegment = selector.GetFocusSegment();

    bool success = viewGroupState_->GetGrowing()->GrowBreadthFirstSearch(
        selector, threshold,
        viewGroupState_->Segmentation().Segments()->GetAdjacencyMap());

    // reset the focus element to the first element that was clicked on
    selector.SetFocusSegment(focusSegment);
    selector.UpdateSelectionNow();
    return success;
  }


};

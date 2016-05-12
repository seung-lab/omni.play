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
        return growToThreshold(mouseEvent->x(), mouseEvent->y());
      case (int)Qt::RightButton | (int)Qt::ShiftModifier:
      case (int)Qt::LeftButton | (int)Qt::ControlModifier:
      case (int)Qt::LeftButton | (int)Qt::ControlModifier
        | (int)Qt::ShiftModifier:
        return trim(mouseEvent->x(), mouseEvent->y());
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
    if (threshold > 1) {
      threshold = 1;
    }

    if (threshold < 0) {
      threshold = 0;
    }

    OmProject::Globals().Users().UserSettings().setGrowThreshold(threshold);
    om::event::UserSettingsUpdated();
    return threshold;
  }

  bool growIncremental(bool isGrowing) {
    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          viewGroupState_->Segmentation().id(), "Grow Selector");

    viewGroupState_->GetGrowing()->GrowIncremental(*selector,
        isGrowing, getUpdatedThreshold(isGrowing),
        viewGroupState_->Segmentation().Segments()->GetAdjacencyMap());
    selector->UpdateSelectionNow();

    return true;
  }

  bool growToThreshold(int x, int y) {
    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      log_infos << "No segment selected" << std::endl;
      return false;
    }

    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          segmentDataWrapper->GetSegmentationID(), "Grow Selector");
    selector->SetFocusSegment(segmentDataWrapper->GetSegmentID());
    double threshold = OmProject::Globals().Users().UserSettings()
      .getGrowThreshold();

    viewGroupState_->GetGrowing()->GrowBreadthFirstSearch(*selector, threshold,
        viewGroupState_->Segmentation().Segments()->GetAdjacencyMap());

    // reset the focus element to the first element that was clicked on
    selector->SetFocusSegment(segmentDataWrapper->GetSegmentID());
    selector->UpdateSelectionNow();
    return true;
  }

  bool trim(int x, int y) {
    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      log_infos << "No segment selected";
      return false;
    }

    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          segmentDataWrapper->GetSegmentationID(), "Grow Selector");

    viewGroupState_->GetGrowing()->Trim(*selector,
        segmentDataWrapper->GetSegmentID(), 0,
        viewGroupState_->Segmentation().Segments()->GetAdjacencyMap());

    selector->UpdateSelectionNow();
    return true;
  }
};

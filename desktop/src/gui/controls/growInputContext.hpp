#pragma once

#include "precomp.h"
#include "gui/controls/inputContext.hpp"
#include "gui/controls/viewStateInputContext.hpp"
#include "gui/controls/findSegment.hpp"
#include "gui/controls/findGlobalCoordinates.hpp"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "segment/actions/omJoinSplitRunner.hpp"
#include "segment/omSegments.h"
#include "segment/omSegmentSelector.h"

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
    std::cout << "button is " << std::hex << button << " modifiers " << std::hex << modifiers << std::endl;
    std::cout << " left button is " << std::hex << Qt::LeftButton << " shift is " << std::hex << Qt::ShiftModifier << std::endl;
    switch ((int)button | (int)modifiers) {
      case (int)Qt::LeftButton:
      case (int)Qt::LeftButton | (int)Qt::ShiftModifier:
        return growToThreshold(mouseEvent->x(), mouseEvent->y());
      case (int)Qt::RightButton | (int)Qt::ShiftModifier:
      case (int)Qt::LeftButton | (int)Qt::ControlModifier:
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
        viewGroupState_->EndSelector();
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

 private:
  om::tool::mode tool_;

  bool growToThreshold(int x, int y) {
    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      return false;
    }

    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          segmentDataWrapper->GetSegmentationID(), "Grow Selector");

    segmentDataWrapper->MakeSegmentationDataWrapper()
      .Segments()->AddSegments_BreadthFirstSearch(
            selector.get(), segmentDataWrapper->GetSegmentID());
    selector->UpdateSelectionNow();
    return true;
  }

  bool trim(int x, int y) {
    boost::optional<SegmentDataWrapper> segmentDataWrapper = 
      findSegmentFunction_(x, y);

    if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
      return false;
    }

    std::shared_ptr<OmSegmentSelector> selector =
      viewGroupState_->GetOrCreateSelector(
          segmentDataWrapper->GetSegmentationID(), "Grow Selector");

    segmentDataWrapper->MakeSegmentationDataWrapper().Segments()->Trim(
        selector.get(), segmentDataWrapper->GetSegmentID());
    return true;
  }
};

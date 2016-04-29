#pragma once
#include "precomp.h"
#include "gui/controls/segmentControlContext.hpp"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"

class JoiningSplittingControls : public SegmentControlContext, public ControlContext {
 public:
  JoiningSplittingControls(OmViewGroupState* viewGroupState,
      SegmentDataWrapper* segmentDataWrapper, om::tool::mode tool)
    : SegmentControlContext(viewGroupState, segmentDataWrapper),
      tool_(tool) {
    }

  virtual bool mouseMoveEvent(QMouseEvent* mouseEvent) override {
    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
    Qt::MouseButton button = mouseEvent->button();
    switch ((int)button | (int)modifiers) {
      case (int)Qt::LeftButton | (int)Qt::ShiftModifier:
        return om::JoinSplitRunner::SelectSegment(
            viewGroupState_, tool_, segmentDataWrapper_);
      default:
        return false;
    }
  }

  virtual bool mousePressEvent(QMouseEvent* mouseEvent) override {
    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
    Qt::MouseButton button = mouseEvent->button();
    switch ((int)button | (int)modifiers) {
      case Qt::LeftButton:
      case (int)Qt::LeftButton | (int)Qt::ShiftModifier:
        return om::JoinSplitRunner::SelectSegment(
            viewGroupState_, tool_, segmentDataWrapper_);
      default:
        return false;
    }
  }

  virtual bool mousePressRelease(QMouseEvent* mouseEvent) {
    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
    Qt::MouseButton button = mouseEvent->button();
    switch ((int)button | (int)modifiers) {
      case Qt::LeftButton:
      case (int)Qt::LeftButton | (int)Qt::ShiftModifier:
        return om::JoinSplitRunner::GoToNextState(viewGroupState_, tool_);
      default:
        return false;
    }
  }

 private:
  om::tool::mode tool_;
};

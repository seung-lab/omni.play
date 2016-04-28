#pragma once
#include "precomp.h"
#include "gui/controls/ToolControlContext.hpp"

class JoiningSplittingControls : public SegmentControlContext {
 public:
  JoiningSplittingControls(OmViewGroupState* viewGroupState,
      SegmentDataWrapper* SegmentDataWrapper, om::tool::mode tool)
    : SegmentControlContext(viewGroupState, segmentDataWrapper),
      tool_(tool) {
    }

  virtual bool mouseMoveEvent(QMouseEvent* mouseEvent) override {
    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
    Qt::MouseButton button = mouseEvent->button();
    switch (button | modifiers) {
      case Qt::LeftButton | Qt::ShiftModifier:
        return om::JoinSplitRunner::SelectSegment(
            *viewGroupState_, tool_, segmentDataWrapper_);
      default:
        return false;
    }
  }

  virtual bool mousePressEvent(QMouseEvent* mouseEvent) override {
    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
    Qt::MouseButton button = mouseEvent->button();
    switch (button | modifiers) {
      case Qt::LeftButton:
      case Qt::LeftButton | Qt::ShiftModifier:
        om::JoinSplitRunner::SelectSegment(
            viewGroupState_, tool, segmentDataWrapper_);
        return true;
      case default:
        return false;
    }
  }

  virtual bool mousePressRelease(QMouseEvent* mouseEvent) {
    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
    Qt::MouseButton button = mouseEvent->button();
    switch (button | modifiers) {
      case Qt::LeftButton:
      case Qt::LeftButton | Qt::ShiftModifier:
        om::JoinSplitRunner::GoToNextState(
            viewGroupState_, tool_);
    }
  }

 private:
  om::tool::mode tool_;
  std::map<int, ToolCommand> mousePressMapping;
}

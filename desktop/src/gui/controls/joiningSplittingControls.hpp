#pragma once
#include "precomp.h"
#include "gui/controls/controlContext.hpp"
#include "gui/controls/viewStateControl.hpp"
#include "gui/controls/findSegmentControl.hpp"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"

class JoiningSplittingControls : public ControlContext,
  public ViewStateControl, public FindSegmentControl {
 public:
  JoiningSplittingControls(OmViewGroupState* viewGroupState,
      SegmentDataWrapper* segmentDataWrapper, om::tool::mode tool)
    : ViewStateControl(viewGroupState, segmentDataWrapper), tool_(tool) {
    }

/*
 *  virtual bool mouseMoveEvent(QMouseEvent* mouseEvent) override {
 *    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
 *    Qt::MouseButton button = mouseEvent->button();
 *    std::cout << "mosuemove event " << std::endl;
 *    switch ((int)button | (int)modifiers) {
 *      case (int)Qt::LeftButton | (int)Qt::ShiftModifier:
 *        return om::JoinSplitRunner::SelectSegment(
 *            viewGroupState_, tool_, segmentDataWrapper_);
 *      default:
 *        return false;
 *    }
 *  }
 *
 *  virtual bool mousePressEvent(QMouseEvent* mouseEvent) override {
 *    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
 *    Qt::MouseButton button = mouseEvent->button();
 *    switch ((int)button | (int)modifiers) {
 *      case Qt::LeftButton:
 *      case (int)Qt::LeftButton | (int)Qt::ShiftModifier:
 *        return om::JoinSplitRunner::SelectSegment(
 *            viewGroupState_, tool_, segmentDataWrapper_);
 *      default:
 *        return false;
 *    }
 *  }
 *
 *  virtual bool mousePressRelease(QMouseEvent* mouseEvent) {
 *    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
 *    Qt::MouseButton button = mouseEvent->button();
 *    switch ((int)button | (int)modifiers) {
 *      case Qt::LeftButton:
 *      case (int)Qt::LeftButton | (int)Qt::ShiftModifier:
 *        return om::JoinSplitRunner::GoToNextState(viewGroupState_, tool_);
 *      default:
 *        return false;
 *    }
 *  }
 *
 */
 private:
  om::tool::mode tool_;
};

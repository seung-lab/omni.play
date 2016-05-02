#pragma once
#include "precomp.h"
#include "gui/controls/inputContext.hpp"
#include "gui/controls/viewStateInputContext.hpp"
#include "gui/controls/findSegment.hpp"
#include "gui/controls/findGlobalCoordinates.hpp"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"

class JoiningSplittingInputContext
: public InputContext,
  public ViewStateInputContext,
  public FindSegment {
 public:
  JoiningSplittingInputContext(OmViewGroupState* viewGroupState,
      om::tool::mode tool,
      std::function<std::shared_ptr<SegmentDataWrapper>(int, int)>
        findSegmentFunction)
    : ViewStateInputContext(viewGroupState, segmentDataWrapper), tool_(tool),
      FindSegment(findSegmentFunction) {
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

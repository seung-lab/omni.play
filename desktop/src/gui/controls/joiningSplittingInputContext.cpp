#pragma once
#include "precomp.h"
#include "gui/controls/inputContext.hpp"
#include "gui/controls/viewStateInputContext.hpp"
#include "gui/controls/findSegment.hpp"
#include "gui/controls/findGlobalCoordinates.hpp"
#include "gui/controls/joiningSplittingInputContext.hpp"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "segment/actions/omJoinSplitRunner.hpp"

JoiningSplittingInputContext::JoiningSplittingInputContext(
    OmViewGroupState* viewGroupState,
    om::tool::mode tool,
    std::function<boost::optional<SegmentDataWrapper>(int, int)>
      findSegmentFunction)
  : ViewStateInputContext(viewGroupState), tool_(tool),
    FindSegment(findSegmentFunction) {
  }

bool JoiningSplittingInputContext::mousePressEvent(QMouseEvent* mouseEvent) {
  Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
  Qt::MouseButton button = mouseEvent->button();
  switch ((int)button | (int)modifiers) {
    case Qt::LeftButton:
    case (int)Qt::LeftButton | (int)Qt::ShiftModifier:
      return om::JoinSplitRunner::SelectSegment(
          *viewGroupState_, tool_,
          findSegmentFunction_(mouseEvent->x(), mouseEvent->y()));
    default:
      return false;
  }
}

bool JoiningSplittingInputContext::mouseMoveEvent(QMouseEvent* mouseEvent) {
  Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
  Qt::MouseButtons buttons = mouseEvent->buttons();
  switch ((int)buttons | (int)modifiers) {
    case (int)Qt::LeftButton:
    case (int)Qt::LeftButton | (int)Qt::ShiftModifier:
      return om::JoinSplitRunner::SelectSegment(
          *viewGroupState_, tool_,
          findSegmentFunction_(mouseEvent->x(), mouseEvent->y()));
    default:
      return false;
  }
}

bool JoiningSplittingInputContext::mouseReleaseEvent(QMouseEvent* mouseEvent) {
  Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
  Qt::MouseButton button = mouseEvent->button();
  switch ((int)button | (int)modifiers) {
    case Qt::LeftButton:
      return om::JoinSplitRunner::FinishSelectingSegments(*viewGroupState_,
          tool_);
    default:
      return false;
  }
}

bool JoiningSplittingInputContext::keyReleaseEvent(QKeyEvent *keyEvent) {
  Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
  int key = keyEvent->key();
  switch (key | (int) modifiers) {
    case Qt::Key_Shift:
      return om::JoinSplitRunner::FinishSelectingSegments(*viewGroupState_,
          tool_);
    default:
      return false;
  }
}

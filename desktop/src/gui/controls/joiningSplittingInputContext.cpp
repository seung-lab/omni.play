#pragma once
#include "precomp.h"
#include "gui/controls/inputContext.hpp"
#include "gui/controls/viewInputContext.hpp"
#include "gui/viewGroup/viewInputConversion.hpp"
#include "gui/controls/joiningSplittingInputContext.hpp"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "segment/actions/omJoinSplitRunner.hpp"

JoiningSplittingInputContext::JoiningSplittingInputContext(
    ViewInputConversion* viewInputConversion,
    OmViewGroupState* viewGroupState, om::tool::mode tool)
  : ViewInputContext(viewInputConversion, viewGroupState), tool_(tool) {
  }

bool JoiningSplittingInputContext::mousePressEvent(QMouseEvent* mouseEvent) {
  Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
  Qt::MouseButton button = mouseEvent->button();
  switch ((int)button | (int)modifiers) {
    case Qt::LeftButton:
    case (int)Qt::LeftButton | (int)Qt::ShiftModifier:
      return om::JoinSplitRunner::SelectSegment(
          *viewGroupState_, tool_, viewInputConversion_->FindSegment(
            mouseEvent->x(), mouseEvent->y()));
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
          *viewGroupState_, tool_, viewInputConversion_->FindSegment(
            mouseEvent->x(), mouseEvent->y()));
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

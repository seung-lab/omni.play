#pragma once
#include "precomp.h"

#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "segment/actions/omJoinSplitRunner.hpp"

class OmMouseEventRelease {
 private:
  OmView2dState* const state_;

  om::coords::Global dataClickPoint_;

 public:
  OmMouseEventRelease(OmView2dState* state)
      : state_(state) {}

  void Release(QMouseEvent* event) {
    setState(event);

    state_->setScribbling(false);
    state_->SetLastDataPoint(dataClickPoint_);
    om::tool::mode tool = OmStateManager::GetToolMode();

    switch (tool) {
      case om::tool::PAINT:
      case om::tool::ERASE:
      case om::tool::LANDMARK:
        state_->OverrideToolModeForPan(false);
        break;
      case om::tool::SELECT:
        if (OmBrushSelect::IsSelecting(state_)) {
          OmBrushSelect::EndSelector(state_);
        }
        state_->OverrideToolModeForPan(false);
        break;
      case om::tool::JOIN:
      case om::tool::SPLIT:
      case om::tool::MULTISPLIT:
        om::JoinSplitRunner::GoToNextState(
            state_->getViewGroupState(), tool);
      default:
        break;
    }
  }

 private:
  void setState(QMouseEvent* event) {
    om::coords::Screen clicked(Vector2i(event->x(), event->y()),
                               state_->Coords());
    dataClickPoint_ = clicked.ToGlobal();
  }
};

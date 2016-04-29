#pragma once
#include "precomp.h"

#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "segment/actions/omJoinSplitRunner.hpp"
#include "gui/controls/controlContext.hpp"
#include "gui/controls/joiningSplittingControls.hpp"

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

    std::unique_ptr<ControlContext> context;
    switch (tool) {
      case om::tool::JOIN:
      case om::tool::SPLIT:
      case om::tool::MULTISPLIT:
        context = std::unique_ptr<ControlContext> {
          std::make_unique<JoiningSplittingControls>(
              &state_->getViewGroupState(),
              om::mouse::event::getSelectedSegment(
                *state_, dataClickPoint_).get_ptr(),
              tool) };
        //context = std::unique_ptr<ControlContext> {
          //std::make_unique<JoiningSplittingControls>(state_,
              //static_cast<SegmentDataWrapper*>(nullptr), tool) };
      default:
        break;
        // fallthrough
    }

    if (context->mouseReleaseEvent(event)) {
      //default camera control context
      return;
    }


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

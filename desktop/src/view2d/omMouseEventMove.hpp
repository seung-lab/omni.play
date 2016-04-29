#pragma once
#include "precomp.h"

#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "utility/dataWrappers.h"
#include "view2d/brush/omBrushSelect.hpp"
#include "view2d/brush/omBrushPaint.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "gui/controls/controlContext.hpp"
#include "gui/controls/joiningSplittingControls.hpp"

class OmMouseEventMove {
 private:
  OmView2d* const v2d_;
  OmView2dState* const state_;

  bool controlKey_;
  bool altKey_;
  bool shiftKey_;
  bool leftMouseButton_;
  bool rightMouseButton_;
  bool middleMouseButton_;
  om::tool::mode tool_;
  QMouseEvent* event_;
  om::coords::Global dataClickPoint_;

  friend class OmMouseEventState;

 public:
  OmMouseEventMove(OmView2d* v2d, OmView2dState* state)
      : v2d_(v2d), state_(state) {}

  void Move(QMouseEvent* event) {
    setState(event);

    state_->SetMousePoint(event);

    std::unique_ptr<ControlContext> context;
    switch (tool_) {
      case om::tool::JOIN:
      case om::tool::SPLIT:
      case om::tool::MULTISPLIT:
        context = std::unique_ptr<ControlContext> {
          std::make_unique<JoiningSplittingControls>(
              &state_->getViewGroupState(),
              om::mouse::event::getSelectedSegment(
                *state_, dataClickPoint_).get_ptr(),
              tool_) };
        break;
      default:
        break;
        // default camera context controls
    }
    if (context && context->mouseMoveEvent(event)) {
      return;
    }


    if (leftMouseButton_) {
      if (shouldPan()) {
        doPan();
        return;
      }

      switch (tool_) {
        case om::tool::SELECT:
          selectSegments();
          state_->SetLastDataPoint(dataClickPoint_);
          break;

        case om::tool::PAN:
          doPan();
          break;

        case om::tool::PAINT:
          if (state_->getScribbling()) {
            paint();
          }
          state_->SetLastDataPoint(dataClickPoint_);
          break;

        case om::tool::ERASE:
          if (state_->getScribbling()) {
            erase();
          }
          state_->SetLastDataPoint(dataClickPoint_);
          break;
        case om::tool::JOIN:
        case om::tool::SPLIT:
        case om::tool::MULTISPLIT:
          om::mouse::event::doJoinSplitSegment(*state_, dataClickPoint_,
              tool_);
        default:
          break;
      }
    } else if (middleMouseButton_) {
      if (shouldPan()) {
        doPan();
      }
    }

    // redraw for custom mouse cursor brush
    v2d_->Redraw();
  }

 private:
  inline bool shouldPan() const {
    return controlKey_ || state_->OverrideToolModeForPan();
  }

  inline void doPan() {
    mousePan();
    om::event::Redraw3d();
  }

  inline void setState(QMouseEvent* event) {
    event_ = event;

    controlKey_ = event->modifiers() & Qt::ControlModifier;
    shiftKey_ = event->modifiers() & Qt::ShiftModifier;
    altKey_ = event->modifiers() & Qt::AltModifier;

    leftMouseButton_ = event->buttons() & Qt::LeftButton;
    rightMouseButton_ = event->buttons() & Qt::RightButton;
    middleMouseButton_ = event->buttons() & Qt::MiddleButton;

    tool_ = OmStateManager::GetToolMode();
    om::coords::Screen clicked(Vector2i(event->x(), event->y()),
                               state_->Coords());
    dataClickPoint_ = clicked.ToGlobal();
  }

  inline void selectSegments() {
    om::common::AddOrSubtract addOrSubtractSegments =
        altKey_ ? om::common::AddOrSubtract::SUBTRACT
                : om::common::AddOrSubtract::ADD;
      OmBrushSelect::ContinueSelector(state_, dataClickPoint_,
                                  addOrSubtractSegments);
  }

  void erase() { OmBrushPaint::PaintByLine(state_, dataClickPoint_, 0); }

  void paint() {
    const om::common::SegID segmentValueToPaint = state_->GetSegIDForPainting();
    assert(segmentValueToPaint);
    OmBrushPaint::PaintByLine(state_, dataClickPoint_, segmentValueToPaint);
  }

  inline void mousePan() {
    state_->DoMousePan(
        om::coords::Screen(event_->x(), event_->y(), state_->Coords()));
  }
};

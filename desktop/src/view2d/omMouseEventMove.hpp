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
        default:
          break;
      }
    } else if (middleMouseButton_) {
      if (shouldPan()) {
        doPan();
      }
    } else {
      // we only need to redraw if the other actions haven't. (they all trigger redraws)
      v2d_->Redraw();
    }

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
    if (altKey_) {
      OmBrushSelect::StartOrContinueSelector(state_, dataClickPoint_,
                                  om::common::AddOrSubtract::SUBTRACT);

    } else {
      OmBrushSelect::StartOrContinueSelector(state_, dataClickPoint_,
                                  om::common::AddOrSubtract::ADD);
    }
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

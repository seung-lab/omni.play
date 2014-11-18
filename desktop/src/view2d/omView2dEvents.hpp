#pragma once
#include "precomp.h"

#include "events/details/segmentEvent.h"
#include "events/listeners.h"
#include "gui/widgets/omCursors.h"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"

// deal w/ Omni events
class OmView2dEvents : public om::event::SegmentEventListener,
                       public om::event::View2dEventListener,
                       public om::event::ToolModeEventListener {
 private:
  OmView2d* const v2d_;
  OmView2dState* const state_;

 public:
  OmView2dEvents(OmView2d* v2d, OmView2dState* state)
      : v2d_(v2d), state_(state) {}

  void SegmentModificationEvent(om::event::SegmentEvent*) { v2d_->Redraw(); }

  void SegmentGUIlistEvent(om::event::SegmentEvent*) {}

  void SegmentSelectedEvent(om::event::SegmentEvent*) {}

  void SegmentBrushEvent(om::event::SegmentEvent*) {}

  void ViewBoxChangeEvent() {
    state_->Coords().UpdateTransformationMatrices();
    v2d_->Redraw();
  }

  void ViewPosChangeEvent() {
    state_->Coords().UpdateTransformationMatrices();
    v2d_->Redraw();
  }

  void ViewCenterChangeEvent() {
    state_->Coords().UpdateTransformationMatrices();
    state_->ChangeViewCenter();
    v2d_->Redraw();
    om::event::Redraw3d();
  }

  void ViewRedrawEvent() {
    state_->Coords().UpdateTransformationMatrices();
    v2d_->Redraw();
  }

  void ViewBlockingRedrawEvent() {
    state_->Coords().UpdateTransformationMatrices();
    v2d_->RedrawBlocking();
  }

  void ToolModeChangeEvent() { OmCursors::setToolCursor(v2d_); }

  void CoordSystemChangeEvent() {}
};

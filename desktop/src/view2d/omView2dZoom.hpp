#pragma once
#include "precomp.h"

#include "view2d/omView2dState.hpp"

class OmView2dZoom {
 private:
  OmView2dState& state_;
  OmZoomLevel& zoomLevel_;

 public:
  OmView2dZoom(OmView2dState& state)
      : state_(state), zoomLevel_(state.ZoomLevel()) {}

  void MouseLeftButtonClick(QMouseEvent* event) {
    const bool zoomOut = event->modifiers() & Qt::ControlModifier;

    if (zoomOut) {
      doMouseZoom(-15);
    } else {
      doMouseZoom(15);
    }
  }

  void MouseWheelZoom(const int numSteps) { doMouseZoom(numSteps); }

  void KeyboardZoomIn() { doMouseZoom(1); }

  void KeyboardZoomOut() { doMouseZoom(-1); }

 private:
  void doMouseZoom(const int numSteps) {
    zoomLevel_.MouseWheelZoom(numSteps, state_.IsLevelLocked());

    om::event::ViewPosChanged();
    state_.SetViewSliceOnPan();
    om::event::ViewCenterChanged();
  }
};

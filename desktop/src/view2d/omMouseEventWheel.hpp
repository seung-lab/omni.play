#pragma once

#include "gui/toolbars/mainToolbar/filterWidget.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2dZoom.hpp"

class OmMouseEventWheel {
 private:
  OmView2d* const v2d_;
  OmView2dState* const state_;

 public:
  OmMouseEventWheel(OmView2d* v2d, OmView2dState* state)
      : v2d_(v2d), state_(state) {}

  void Wheel(QWheelEvent* event) {
    // Blanchette/Summerfield second edition page 482
    const int numDegrees = event->delta() / 8;
    const int numSteps = numDegrees / 15;

    const bool controlKey = event->modifiers() & Qt::ControlModifier;
    const bool shiftKey = event->modifiers() & Qt::ShiftModifier;
    const bool altKey = event->modifiers() & Qt::AltModifier;

    const bool moveThroughStack = controlKey;
    const bool moveThroughStackSlow = controlKey && altKey;
    const bool changeAlpha = shiftKey;
    const bool changeBrushSize = controlKey && shiftKey;

    if (changeBrushSize) {
      static const int BrushInc = 4;

      if (numSteps >= 0) {
        OmStateManager::BrushSize()->IncreaseSize(BrushInc);
      } else {
        OmStateManager::BrushSize()->DecreaseSize(BrushInc);
      }

    } else if (moveThroughStackSlow) {
      if (numSteps >= 0) {
        state_->MoveUpStackCloserToViewer();
      } else {
        state_->MoveDownStackFartherFromViewer();
      }

    } else if (moveThroughStack) {
      if (numSteps >= 0) {
        state_->MoveUpStackCloserToViewer(Om2dPreferences::ScrollRate());
      } else {
        state_->MoveDownStackFartherFromViewer(Om2dPreferences::ScrollRate());
      }

    } else if (changeAlpha) {

      if (numSteps >= 0) {
        FilterWidget::IncreaseAlpha();
      } else {
        FilterWidget::DecreaseAlpha();
      }

    } else {
      v2d_->Zoom()->MouseWheelZoom(numSteps);
    }

    event->accept();

    OmEvents::Redraw3d();
  }
};

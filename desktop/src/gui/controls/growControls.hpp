#pragma once
#include "precomp.h"
//#include "gui/controls/ToolControlContext.hpp"

/*
 *class GrowControls : public ToolControlContext {
 * public:
 *  GrowControls(OmViewGroupState viewGroupState,
 *      SegmentDataWrapper SegmentDataWrapper)
 *    : ToolControlContext(viewGroupState, segmentDataWrapper,
 *        om::tool::mode::GROW) {
 *    }
 *
 *  void mousePressEvent(QMouseEvent *mouseEvent) {
 *    Qt::KeyboardModifiers modifiers = mouseEvent->modifiers();
 *    Qt::MouseButton button = mouseEvent->button();
 *    switch (button | modifiers) {
 *      case Qt::LeftButton | Qt::ControlModifier:
 *        trim();
 *        break;
 *      case Qt::LeftButton | Qt::ShiftModifier:
 *        growAndHold();
 *        break;
 *      case Qt::LeftButton:
 *        grow();
 *        break;
 *      case default:
 *        return;
 *    }
 *  }
 *
 *  void mouseWheelEvent(QWheelEvent *wheelEvent) {
 *    Qt::KeyboardModifiers modifiers = wheelEvent->modifiers();
 *    int numDegrees = wheelEvent ->delta() / 8;
 *    int numSteps = numDegrees / 15;
 *    if (modifiers & Qt::ShiftModifier) {
 *      if (numSteps > 0) {
 *        growUpOne();
 *      } else {
 *        trimDownOne();
 *      }
 *    }
 *  }
 *
 *  void keyReleaseEvent(QKeyEvent *keyEvent) {
 *    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
 *    Qt::Key key = event->key();
 *    switch (key) {
 *      case Qt::Key_Shift:
 *        commitGrow();
 *        break;
 *      default:
 *        return;
 *    }
 *  }
 *
 *  void trim() {
 *    SegmentationDataWrapper segmentationDataWrapper =
 *      segmentDataWrapper.MakeSegmentationDataWrapper();
 *    OmSegmentSelector selector(segmentationDataWrapper, this, "Trim" );
 *    segmentationDataWrapper.Segments()->Trim(&selector, pickPoint.sdw.GetSegmentID());
 *  }
 *
 *  void TrimDown() {
 *  }
 *
 *  void GrowUp() {
 *  }
 *
 *  void ActivateGrow() {
 *  }
 *
 *  void DeactivateGrow() {
 *  }
 *
 *  void GrowToThreshold() {
 *    SegmentationDataWrapper segmentationDataWrapper =
 *      segmentDataWrapper.MakeSegmentationDataWrapper();
 *    OmSegmentSelector selector(segmentationDataWrapper, this, "Trim" );
 *    segmentationDataWrapper.Segments()->
 *      AddSegments_BreadthFirstSearch(&selector, pickPoint.sdw.GetSegmentID());
 *  }
 *
 *  void Commit() {
 *  }
 *
 * private:
 *  std::map<int, ToolCommand> mousePressMapping;
 *
 *}
 */

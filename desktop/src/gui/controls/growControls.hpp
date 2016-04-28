#pragma once
#include "precomp.h"
#include "gui/controls/ToolControlContext.hpp"

class GrowControls : public ToolControlContext {
 public:
  GrowControls(OmViewGroupState viewGroupState,
      SegmentDataWrapper SegmentDataWrapper)
    : ToolControlContext(viewGroupState, segmentDataWrapper,
        om::tool::mode::GROW) {
    }

  void mousePressEvent(QMouseEvent *event) {
    Qt::KeyboardModifiers modifiers = event->modifiers();
    Qt::MouseButton button = event->button();
  }

  void mouseReleaseEvent(QMouseEvent *event) {
    Qt::KeyboardModifiers modifiers = event->modifiers();
    Qt::MouseButton button = event->button();
  }

  void mouseWheelEvent(QMouseEvent *event) {
    Qt::KeyboardModifiers modifiers = event->modifiers();
    Qt::MouseButton button = event->button();
  }

  void keyPressEvent(QMouseEvent *event) {
    Qt::KeyboardModifiers modifiers = event->modifiers();
    Qt::Key key = event->key();
  }

  void keyReleaseEvent(QMouseEvent *event) {
    Qt::KeyboardModifiers modifiers = event->modifiers();
    Qt::Key key = event->key();
  }

  class TrimCommand : public ToolCommand {
   public:
    TrimCommand(OmViewGroupState& viewGroupState,
        SegmentDataWrapper& segmentDataWrapper)
      : ToolCommand(viewGroupState, segmentDataWrapper) {}

    virtual void execute(bool activated) {
      SegmentationDataWrapper segmentationDataWrapper =
        segmentDataWrapper.MakeSegmentationDataWrapper();
      OmSegmentSelector selector(segmentationDataWrapper, this, "Trim" );
      segmentationDataWrapper.Segments()->Trim(&selector, pickPoint.sdw.GetSegmentID());
    }
  }

  void Trim() {
    SegmentationDataWrapper segmentationDataWrapper =
      segmentDataWrapper.MakeSegmentationDataWrapper();
    OmSegmentSelector selector(segmentationDataWrapper, this, "Trim" );
    segmentationDataWrapper.Segments()->Trim(&selector, pickPoint.sdw.GetSegmentID());
  }

  void TrimDown() {
  }

  void GrowUp() {
  }

  void ActivateGrow() {
  }

  void DeactivateGrow() {
  }

  void GrowToThreshold() {
    SegmentationDataWrapper segmentationDataWrapper =
      segmentDataWrapper.MakeSegmentationDataWrapper();
    OmSegmentSelector selector(segmentationDataWrapper, this, "Trim" );
    segmentationDataWrapper.Segments()->
      AddSegments_BreadthFirstSearch(&selector, pickPoint.sdw.GetSegmentID());
  }

  void Commit() {
  }

 private:
  std::map<int, ToolCommand> mousePressMapping;

}

#pragma once
#include "precomp.h"

/*
 * The tool mode is the input context for control. It may define any of the 
 * 3 kinds of input:
 * 1. Actions
 * 2. States
 * 3. Range
 */
class ToolControlContext {
 public:
  ToolControl(OmViewGroupState viewGroupState,
      SegmentDataWrapper segmentDataWrapper, om::tool::mode tool)
    : viewGroupState_(viewGroupState), tool_(tool),
      segmentDataWrapper_(segmentDataWrapper) {
    }

  void mapFunctionX(key, function) {
    functionMap.insert(std::pair(key,function);
  }

  void callFunction(key) {
    if (functionMap.find(key) != functionMap.end()) {
      functionMap[key]();
    }
  }

  void mouseMoveEvent(QMouseEvent *event) {
    QT::KeyboardModifier modifiers = event->modifiers();
  }
  void mousePressEvent(QMouseEvent *event) {}
  void mouseReleaseEvent(QMouseEvent *event) {}
  void mouseWheelEvent(QMouseEvent *event) {}
  void keyPressEvent(QKeyEvent *event) {}
  void keyReleaseEvent(QKeyEvent *event) {}

 private:
  OmViewGroupState viewGroupState_;
  om::tool::mode tool_;
  SegmentDataWrapper segmentDataWrapper_;

  std::map<key, ToolCommand> keyToCommandMapping;
}

class ToolCommand {
 public:
  ToolCommand(OmViewGroupState& viewGroupState,
      SegmentDataWrapper& segmentDataWrapper)
    : viewGroupState_(viewGroupState),
      segmentDataWrapper_(segmentDataWrapper) {}
  virtual void execute(bool activated) = 0;
  OmViewGroupState& viewGroupState_;
  SegmentDataWrapper& segmentDataWrapper_;
}

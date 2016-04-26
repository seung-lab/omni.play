#pragma once
#include "precomp.h"

/*
 * The tool mode is the input context for control. It may define any of the 
 * 3 kinds of input:
 * 1. Actions
 * 2. States
 * 3. Range
 */
class ToolControl {
 public:
  ToolControl(OmViewGroupState viewGroupState, om::tool::mode tool,
      SegmentDataWrapper segmentDataWrapper);

  virtual mouseMoveEvent(QMouseEvent *event);
  virtual mousePressEvent(QMouseEvent *event);
  virtual mouseReleaseEvent(QMouseEvent *event);
  virtual mouseWheelEvent(QMouseEvent *event);
  virtual keyPressEvent(QKeyEvent *event);
  virtual keyReleaseEvent(QKeyEvent *event);

 private:

}

#pragma once
#include "precomp.h"

/*
 * The tool mode is the input context for control.
 */
class ControlContext {
 public:
  // allow implementation cleanup
  virtual ~ControlContext();

  // prevent copying
  ControlContext(ControlContext const &) = delete;
  ControlContext& operator=(ControlContext const &) = delete;

  /*
   * Virtual functions to handle every type of input.
   * Returns:
   *  true - mapped to a valid command
   *  false - no command found
   */
  virtual bool mouseMoveEvent(QMouseEvent *mouseEvent) { return false; }
  virtual bool mousePressEvent(QMouseEvent *mouseEvent) { return false; }
  virtual bool mouseReleaseEvent(QMouseEvent *mouseEvent) { return false; }
  virtual bool mouseWheelEvent(QWheelEvent *wheelEvent) { return false; }
  virtual bool keyPressEvent(QKeyEvent *keyEvent) { return false; }
  virtual bool keyReleaseEvent(QKeyEvent *keyEvent) { return false; }

 protected:
  // disallow base class construction
  ToolControlContext() {}
}

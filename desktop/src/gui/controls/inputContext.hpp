#pragma once
#include "precomp.h"

/*
 * The tool mode is the input context for control.
 */
class InputContext {
 public:
  // allow implementation cleanup
  virtual ~InputContext() = default;
  // prevent copying (Rule of 5)
  InputContext(InputContext const &) = delete;
  InputContext(InputContext &&) = delete;
  InputContext& operator=(InputContext const &) = delete;
  InputContext& operator=(InputContext &&) = delete;

  /*
   * Virtual functions to handle every type of input.
   * Returns:
   *  true - mapped to a valid command
   *  false - no command found
   */
  virtual bool mouseMoveEvent(QMouseEvent *mouseEvent) { return false; }
  virtual bool mousePressEvent(QMouseEvent *mouseEvent) { return false; }
  virtual bool mouseReleaseEvent(QMouseEvent *mouseEvent) { return false; }
  virtual bool mouseDoubleClickEvent(QMouseEvent *mouseEvent) { return false; }
  virtual bool wheelEvent(QWheelEvent *wheelEvent) { return false; }
  virtual bool keyPressEvent(QKeyEvent *keyEvent) { return false; }
  virtual bool keyReleaseEvent(QKeyEvent *keyEvent) { return false; }

 protected:
  // disallow base class construction
  InputContext() {}
};

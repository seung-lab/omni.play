#pragma once

#include "viewGroup/omViewGroupState.h"
#include "gui/controls/inputContext.hpp"
#include "gui/controls/viewInputContext.hpp"
#include "gui/viewGroup/viewInputConversion.hpp"
#include "gui/controls/growInputContext.hpp"

/*
 * Facade class that determines the correct input context and forwards
 * the control event to that correct input context.
 */
class ViewControls :
  public InputContext,
  public ViewInputContext {
 public:
  ViewControls(ViewInputConversion* viewInputConversion,
      OmViewGroupState* viewGroupState);
  virtual ~ViewControls() = default;

  virtual bool mousePressEvent(QMouseEvent* mouseEvent) override;
  virtual bool mouseMoveEvent(QMouseEvent* mouseEvent) override;
  virtual bool mouseReleaseEvent(QMouseEvent* mouseEvent) override;
  virtual bool mouseDoubleClickEvent(QMouseEvent* mouseEvent) override;
  virtual bool wheelEvent(QWheelEvent* wheelEvent) override;
  virtual bool keyPressEvent(QKeyEvent* keyEvent) override;
  virtual bool keyReleaseEvent(QKeyEvent* keyEvent) override;

 private:
  /*
   * Get the correct input context that should be active
   */
  std::unique_ptr<InputContext> getToolInputContext();

  /*
   * Wrapper method to fetch the input context and only execute
   * if it exists
   */
  bool runEventAction(std::function<bool(InputContext&)> eventAction);
};

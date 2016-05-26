#pragma once

#include "viewGroup/omViewGroupState.h"
#include "gui/viewGroup/viewInputConversion.hpp"
#include "gui/controls/growInputContext.hpp"

/*
 * Base class for controls that are used for views.
 * This class provides functions that convert x and y controls
 * to necessary components the actions can act on.
 */
class ViewControls {
 public:
  ViewControls(ViewInputConversion* viewInputConversion,
      OmViewGroupState* viewGroupState);
  virtual ~ViewControls() = default;

  bool mousePressEvent(QMouseEvent* mouseEvent);
  bool mouseMoveEvent(QMouseEvent* mouseEvent);
  bool mouseReleaseEvent(QMouseEvent* mouseEvent);
  bool mouseDoubleClickEvent(QMouseEvent* mouseEvent);
  bool wheelEvent(QWheelEvent* wheelEvent);
  bool keyPressEvent(QKeyEvent* keyEvent);
  bool keyReleaseEvent(QKeyEvent* keyEvent);

 private:
  /*
   * Get the correct input context that should be active
   */
  std::unique_ptr<InputContext> getToolInputContext();

  /*
   * A helper method to forward the event to the the InputContext controls
   */
  bool runEventAction(std::function<bool(InputContext&)> eventAction);

  OmViewGroupState* viewGroupState_;

  std::function<boost::optional<SegmentDataWrapper>(int, int)>
    findSegmentFunction_;

  std::function<boost::optional<om::coords::Global>(int, int)>
    findGlobalCoordsFunction_;
};

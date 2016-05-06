#pragma once

#include "viewGroup/omViewGroupState.h"
#include "gui/viewGroup/viewInputConversion.hpp"
#include "gui/controls/growInputContext.hpp"

class ViewControls {
 public:
  ViewControls(ViewInputConversion* viewInputConversion,
      OmViewGroupState* viewGroupState);

  bool mousePressEvent(QMouseEvent* mouseEvent);
  bool mouseMoveEvent(QMouseEvent* mouseEvent);
  bool mouseReleaseEvent(QMouseEvent* mouseEvent);
  bool mouseDoubleClickEvent(QMouseEvent* mouseEvent);
  bool wheelEvent(QWheelEvent* wheelEvent);
  bool keyPressEvent(QKeyEvent* keyEvent);
  bool keyReleaseEvent(QKeyEvent* keyEvent);

 private:
  std::unique_ptr<InputContext> getToolInputContext();
  bool runEventAction(std::function<bool(InputContext&)> eventAction);

  ViewInputConversion* viewInputConversion_;
  OmViewGroupState* viewGroupState_;
  std::function<boost::optional<SegmentDataWrapper>(int, int)>
    getSegmentFunction_;
};

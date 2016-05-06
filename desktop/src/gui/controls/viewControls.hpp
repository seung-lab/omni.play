#pragma once

#include "viewGroup/omViewGroupState.h"
#include "gui/controls/inputContext.hpp"
#include "gui/viewGroup/viewInputConversion.hpp"
#include "gui/controls/joiningSplittingInputContext.hpp"
#include "gui/controls/growInputContext.hpp"
#include "system/omStateManager.h"
#include "gui/controls/viewControls.hpp"

class ViewControls {
 public:
  ViewControls(ViewInputConversion* viewInputConversion,
      OmViewGroupState* viewGroupState)
    : viewInputConversion_(viewInputConversion),
      viewGroupState_(viewGroupState),
      getSegmentFunction_(std::bind(&ViewInputConversion::GetSelectedSegment,
        viewInputConversion_, std::placeholders::_1, std::placeholders::_2)) {}

  bool keyPressEvent(QKeyEvent* keyEvent) {
    return runEventAction([&keyEvent] (InputContext& inputContext) {
          return inputContext.keyPressEvent(keyEvent);
        });
  }

  bool keyReleaseEvent(QKeyEvent* keyEvent) {
    return runEventAction([&keyEvent] (InputContext& inputContext) {
          return inputContext.keyReleaseEvent(keyEvent);
        });
  }

  bool mouseMoveEvent(QMouseEvent* mouseEvent) {
    return runEventAction([&mouseEvent] (InputContext& inputContext) {
          return inputContext.mouseMoveEvent(mouseEvent);
        });
  }

  bool mousePressEvent(QMouseEvent* mouseEvent) {
    return runEventAction([&mouseEvent] (InputContext& inputContext) {
          return inputContext.mousePressEvent(mouseEvent);
        });
  }

  bool mouseReleaseEvent(QMouseEvent* mouseEvent) {
    return runEventAction([&mouseEvent] (InputContext& inputContext) {
          return inputContext.mouseReleaseEvent(mouseEvent);
        });
  }

  bool mouseDoubleClickEvent(QMouseEvent* mouseEvent) {
    return runEventAction([&mouseEvent] (InputContext& inputContext) {
          return inputContext.mouseDoubleClickEvent(mouseEvent);
        });
  }

  bool wheelEvent(QWheelEvent* wheelEvent) {
    return runEventAction([&wheelEvent] (InputContext& inputContext) {
          return inputContext.wheelEvent(wheelEvent);
        });
  }

 private:
  std::unique_ptr<InputContext> getToolInputContext() {
    std::unique_ptr<InputContext> inputContext;
    om::tool::mode tool = OmStateManager::GetToolMode();
    switch (tool) {
      case om::tool::JOIN:
      case om::tool::SPLIT:
      case om::tool::MULTISPLIT:
        return std::make_unique<JoiningSplittingInputContext>(
            viewGroupState_, tool, getSegmentFunction_);
      case om::tool::GROW:
        return std::make_unique<GrowInputContext>(
            viewGroupState_, tool, getSegmentFunction_);
      default:
        return inputContext;
    }
  }

  bool runEventAction(std::function<bool(InputContext&)> eventAction) {
    std::unique_ptr<InputContext> inputContext = getToolInputContext();
    if (inputContext) {
      return eventAction(*inputContext);
    }
    return false;
  }

  ViewInputConversion* viewInputConversion_;
  OmViewGroupState* viewGroupState_;
  std::function<boost::optional<SegmentDataWrapper>(int, int)>
    getSegmentFunction_;
};

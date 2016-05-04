#pragma once

#include "viewGroup/omViewGroupState.h"
#include "gui/controls/inputContext.hpp"
#include "gui/viewGroup/viewInputConversion.hpp"
#include "gui/controls/joiningSplittingInputContext.hpp"

class ViewControls {
 public:
  ViewControls(ViewInputConversion* viewInputConversion,
      OmViewGroupState* viewGroupState)
    : viewInputConversion_(viewInputConversion),
      viewGroupState_(viewGroupState) {}

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
            viewGroupState_, tool,
            [=] (int x, int y) {
              return viewInputConversion_->GetSelectedSegment(x, y);
            });
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
};

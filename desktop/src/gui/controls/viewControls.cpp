#pragma once

#include "viewGroup/omViewGroupState.h"
#include "gui/controls/inputContext.hpp"
#include "gui/viewGroup/viewInputConversion.hpp"
#include "gui/controls/joiningSplittingInputContext.hpp"
#include "gui/controls/growInputContext.hpp"
#include "system/omStateManager.h"
#include "gui/controls/viewControls.hpp"

ViewControls::ViewControls(ViewInputConversion* viewInputConversion,
    OmViewGroupState* viewGroupState)
  : ViewInputContext(viewInputConversion, viewGroupState) {}

bool ViewControls::mouseMoveEvent(QMouseEvent* mouseEvent) {
  return runEventAction([&mouseEvent] (InputContext& inputContext) {
        return inputContext.mouseMoveEvent(mouseEvent);
      });
}

bool ViewControls::mousePressEvent(QMouseEvent* mouseEvent) {
  return runEventAction([&mouseEvent] (InputContext& inputContext) {
        return inputContext.mousePressEvent(mouseEvent);
      });
}

bool ViewControls::mouseReleaseEvent(QMouseEvent* mouseEvent) {
  return runEventAction([&mouseEvent] (InputContext& inputContext) {
        return inputContext.mouseReleaseEvent(mouseEvent);
      });
}

bool ViewControls::mouseDoubleClickEvent(QMouseEvent* mouseEvent) {
  return runEventAction([&mouseEvent] (InputContext& inputContext) {
        return inputContext.mouseDoubleClickEvent(mouseEvent);
      });
}

bool ViewControls::wheelEvent(QWheelEvent* wheelEvent) {
  return runEventAction([&wheelEvent] (InputContext& inputContext) {
        return inputContext.wheelEvent(wheelEvent);
      });
}

bool ViewControls::keyPressEvent(QKeyEvent* keyEvent) {
  return runEventAction([&keyEvent] (InputContext& inputContext) {
        return inputContext.keyPressEvent(keyEvent);
      });
}

bool ViewControls::keyReleaseEvent(QKeyEvent* keyEvent) {
  return runEventAction([&keyEvent] (InputContext& inputContext) {
        return inputContext.keyReleaseEvent(keyEvent);
      });
}


std::unique_ptr<InputContext> ViewControls::getToolInputContext() {
  std::unique_ptr<InputContext> inputContext;
  om::tool::mode tool = OmStateManager::GetToolMode();
  switch (tool) {
    case om::tool::JOIN:
    case om::tool::SPLIT:
    case om::tool::MULTISPLIT:
      return std::make_unique<JoiningSplittingInputContext>(
          viewInputConversion_, viewGroupState_, tool);
    case om::tool::GROW:
      return std::make_unique<GrowInputContext>(
          viewInputConversion_, viewGroupState_, tool);
    default:
      return inputContext;
  }
}

bool ViewControls::runEventAction(std::function<bool(InputContext&)> eventAction) {
  std::unique_ptr<InputContext> inputContext = getToolInputContext();
  if (inputContext && eventAction) {
    return eventAction(*inputContext);
  }
  return false;
}

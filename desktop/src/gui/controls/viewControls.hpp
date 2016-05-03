#pragma once

#include "viewGroup/omViewGroupState.h"
#include "gui/viewGroup/viewInputConversion.hpp"
#include "gui/controls/joiningSplittingInputContext.hpp"

class ViewControls {
 public:
  ViewControls(ViewInputConversion* viewInputConversion,
      OmViewGroupState& viewGroupState)
    : viewInputConversion_(viewInputConversion),
      viewGroupState_(viewGroupState) {}

  void keyPressEvent(QKeyEvent* keyEvent);
  void keyReleaseEvent(QKeyEvent* keyEvent);
  void mouseMoveEvent(QMouseEvent* mouseEvent);
  void mousePressEvent(QMouseEvent* mouseEvent);
  void mouseReleaseEvent(QMouseEvent* mouseEvent);
  void wheelEvent(QWheelEvent* mouseEvent);

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

  ViewInputConversion* viewInputConversion_;
  OmViewGroupState& viewGroupState_;
};

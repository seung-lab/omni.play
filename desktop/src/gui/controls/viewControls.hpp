#pragma once

public ViewControls {
 public:
  void keyPressEvent(QKeyEvent* keyEvent) {
  void keyReleaseEvent(QKeyEvent* keyEvent);
  void mouseMoveEvent(QMouseEvent* mouseEvent);
  void mousePressEvent(QMouseEvent* mouseEvent);
  void mouseReleaseEvent(QMouseEvent* mouseEvent);
  void wheelEvent(QWheelEvent* mouseEvent);

 private:
  std::unique_ptr<InputContext> OmView2d::getToolInputContext() {
    std::unique_ptr<InputContext> inputContext;
    om::tool::mode tool = OmStateManager::GetToolMode();
    switch (tool) {
      case om::tool::JOIN:
      case om::tool::SPLIT:
      case om::tool::MULTISPLIT:
        return std::make_unique<JoiningSplittingInputContext>(
            state_->getViewGroupState(), tool,
            [=] (int x, int y) { return getSelectedSegment(x, y); });
      default:
        return inputContext;
    }
  }

  OmViewGroupState& viewGroupState;
}

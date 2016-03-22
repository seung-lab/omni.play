#pragma once
#include "precomp.h"
#include "events/listeners.h"
#include "gui/widgets/omButton.hpp"
#include "gui/tools.hpp"
#include "system/omStateManager.h"

class ToolButton : public OmButton, public om::event::ToolModeEventListener {
Q_OBJECT

 public:
  ToolButton(QWidget* mw, const QString& title, const QString& statusTip,
           const om::tool::mode tool, const QString& iconPath = "")
      : OmButton(mw, title, statusTip, true), tool_(tool)  {
    if (!iconPath.isEmpty()) {
      QPushButton::setFlat(true);
      OmButton::setIconAndText(iconPath);
    }
    ToolModeChangeEvent(OmStateManager::GetToolMode());
  }

  const om::tool::mode Tool() {
    return tool_;
  }

  void ToolModeChangeEvent(om::tool::mode eventTool) override {
    QPushButton::setEnabled(OmStateManager::IsEnabled(Tool()));

    bool isCorrectTool = tool_ == eventTool;
    QPushButton::setChecked(isCorrectTool);
    QPushButton::setDown(isCorrectTool);
  }

 private:
  const om::tool::mode tool_;

  void onLeftClick() override {
    if (isChecked()) {
      OmStateManager::SetToolModeAndSendEvent(Tool());
    } else {
      OmStateManager::SetOldToolModeAndSendEvent();
    }
  }

};

#pragma once

#include "common/omCommon.h"
#include "gui/widgets/omButton.hpp"

class ToolButton : public OmButton<QWidget> {
 public:
  ToolButton(QWidget*, const QString& title, const QString& statusTip,
             const om::tool::mode tool, const QString& iconPath);

  inline om::tool::mode getToolMode() const { return mTool; }

 private:
  om::tool::mode mTool;

  void doAction();
};

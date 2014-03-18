#pragma once
#include "precomp.h"

#include "common/common.h"
#include "gui/tools.hpp"

class ToolButton;

class NavAndEditButtonGroup : public QButtonGroup {
  Q_OBJECT;

 public:
  explicit NavAndEditButtonGroup(QWidget*);

  virtual ~NavAndEditButtonGroup() {}

  void EnableModifyingWidgets(const bool toBeEnabled);
  void SetTool(const om::tool::mode tool);

 private
Q_SLOTS:
  void buttonWasClicked(const int id);
  void findAndSetTool(const om::tool::mode tool);

Q_SIGNALS:
  void signalSetTool(const om::tool::mode tool);

 private:
  int addButton(ToolButton* button);
  void makeToolActive(ToolButton* button);

  std::map<int, ToolButton*> allToolsByID_;
  std::map<om::tool::mode, ToolButton*> allToolsByMode_;
};

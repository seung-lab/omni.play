#pragma once

#include <map>
#include "common/common.h"
#include "gui/tools.hpp"

#include <QButtonGroup>

class ToolButton;

class NavAndEditButtonGroup : public QButtonGroup {
  Q_OBJECT;

 public:
  explicit NavAndEditButtonGroup(QWidget*);

  virtual ~NavAndEditButtonGroup() {}

  void SetReadOnlyWidgetsEnabled(const bool toBeEnabled);
  void SetModifyWidgetsEnabled(const bool toBeEnabled);
  void SetTool(const om::tool::mode tool);

 private
Q_SLOTS:
  void buttonWasClicked(const int id);
  void findAndSetTool(const om::tool::mode tool);

Q_SIGNALS:
  void signalSetTool(const om::tool::mode tool);

 private:
  int addButton(ToolButton* button);
  void addNavButton(ToolButton* button);
  void addModifyButton(ToolButton* button);
  void makeToolActive(ToolButton* button);

  std::map<int, ToolButton*> allToolsByID_;

  std::map<om::tool::mode, int> navToolIDsByToolMode_;
  std::map<om::tool::mode, int> modifyToolIDsByToolMode_;
};

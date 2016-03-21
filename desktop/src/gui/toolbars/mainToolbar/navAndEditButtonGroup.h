#pragma once
#include "precomp.h"

#include "common/common.h"
#include "gui/tools.hpp"
#include "gui/widgets/toolButton.hpp"


class NavAndEditButtonGroup : public QButtonGroup {
  Q_OBJECT;

 public:
  explicit NavAndEditButtonGroup(QWidget*);

  virtual ~NavAndEditButtonGroup() {}

 private:
  int addButton(ToolButton* button);
};

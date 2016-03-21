#pragma once
#include "precomp.h"

#include "gui/widgets/viewGroupStateButton.hpp"

class OmViewGroupState;

class JoinAllButton : public ViewGroupStateButton {
  Q_OBJECT;

 public:
  JoinAllButton(QWidget* widget, OmViewGroupState& vgs);

  void onLeftClick();
};

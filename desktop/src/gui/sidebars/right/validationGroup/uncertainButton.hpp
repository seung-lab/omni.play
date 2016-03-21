#pragma once
#include "precomp.h"

#include "gui/widgets/viewGroupStateButton.hpp"

class OmViewGroupState;

class UncertainButton : public ViewGroupStateButton {
 public:
  UncertainButton(QWidget* widget, OmViewGroupState& vgs, bool isUncertain);

 private:
  bool isUncertain_;

  void onLeftClick();
};

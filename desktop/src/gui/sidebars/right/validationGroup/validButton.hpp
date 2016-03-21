#pragma once
#include "precomp.h"

#include "gui/widgets/viewGroupStateButton.hpp"

class OmViewGroupState;

class ValidButton : public ViewGroupStateButton {
 public:
  ValidButton(QWidget* widget, OmViewGroupState& vgs,
      om::common::SetValid setValid);

 private:
  om::common::SetValid setValid_;

 protected:
  void onLeftClick();
};

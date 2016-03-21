#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "gui/brushToolbox/brushToolbox.hpp"

class ShowPaintTools : public OmButton {
 private:
  BrushToolbox* brushToolbox_;

 public:
  ShowPaintTools(QWidget* parent, OmViewGroupState& vgs)
      : OmButton(parent, "", "Show brush tools", true),
        brushToolbox_(new BrushToolbox(parent, vgs)) {
    SetIcon(":/toolbars/mainToolbar/icons/1301504581_emblem-art2.png");
  }

 private:
  void onLeftClick() override {
    if (!isChecked()) {
      brushToolbox_->Show();

    } else {
      brushToolbox_->Hide();
    }
  }
};

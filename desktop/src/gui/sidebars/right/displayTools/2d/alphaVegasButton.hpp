#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "system/omAlphaVegasMode.hpp"

namespace om {
namespace displayTools {

class AlphaVegasButton : public OmButton {
 public:
  AlphaVegasButton(QWidget *dt)
      : OmButton(dt, "Vegas Mode", "cycle alpha slider", true) {}

 private:
  std::unique_ptr<OmAlphaVegasMode> vegas_;

  void onLeftClick() override {
    if (vegas_) {
      vegas_.reset();
    } else {
      vegas_.reset(new OmAlphaVegasMode());
    }
  }
};

}  // namespace displayTools
}  // namespace om

#pragma once

#include "gui/widgets/omButton.hpp"
#include "system/omAlphaVegasMode.hpp"

namespace om {
namespace displayTools {

class AlphaVegasButton : public OmButton<QWidget> {
 public:
  AlphaVegasButton(QWidget *dt)
      : OmButton<QWidget>(dt, "Vegas Mode", "cycle alpha slider", true) {}

 private:
  std::unique_ptr<OmAlphaVegasMode> vegas_;

  void doAction() {
    if (vegas_) {
      vegas_.reset();
    } else {
      vegas_.reset(new OmAlphaVegasMode());
    }
  }
};

}  // namespace displayTools
}  // namespace om

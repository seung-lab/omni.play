#pragma once
#include "precomp.h"

#include "view2d/om2dPreferences.hpp"
#include "gui/widgets/omCheckBoxWidget.hpp"
#include "events/events.h"

class ShowCrosshairsCheckbox : public OmCheckBoxWidget {
 public:
  ShowCrosshairsCheckbox(QWidget* p) : OmCheckBoxWidget(p, "Show crosshairs") {
    set(Om2dPreferences::ShowCrosshairs());
  }

 private:
  void doAction(const bool isChecked) {
    Om2dPreferences::ShowCrosshairs(isChecked);
    om::event::Redraw2d();
  }
};

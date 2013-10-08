#pragma once

#include "view2d/om2dPreferences.hpp"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "events/omEvents.h"

class ScrollRateSpinBox : public OmIntSpinBox {
 public:
  ScrollRateSpinBox(QWidget* p) : OmIntSpinBox(p, om::UPDATE_AS_TYPE) {
    setSingleStep(1);
    setMinimum(1);
    setMaximum(10);
    setValue(Om2dPreferences::ScrollRate());
  }

 private:
  void actUponValueChange(const int val) {
    Om2dPreferences::ScrollRate(val);
    OmEvents::Redraw2d();
  }
};

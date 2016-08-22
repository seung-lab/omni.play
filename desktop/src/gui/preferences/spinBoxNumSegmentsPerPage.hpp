#pragma once
#include "precomp.h"

#include "system/omLocalPreferences.hpp"
#include "gui/widgets/omIntSpinBox.hpp"
#include "events/events.h"
#include "gui/segmentLists/elementListBox.hpp"

class SpinBoxNumSegmentsPerPage : public OmIntSpinBox {
 public:
  SpinBoxNumSegmentsPerPage(QWidget* p) : OmIntSpinBox(p, true) {
    setSingleStep(1);
    setMinimum(1);
    setMaximum(100);
    setValue(OmLocalPreferences::GetNumSegmentsPerPage());
  }

 private:
  void actUponValueChange(const int val) {
    OmLocalPreferences::SetNumSegmentsPerPage(val);
    if (ElementListBox::Widget()) {
       ElementListBox::PopulateLists();
    }
  }
};

#pragma once

#include "actions/omActions.h"
#include "events/listeners.h"
#include "events/events.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/widgets/omDoubleSpinBox.hpp"

#include <limits>

class SizeThresholdSpinBox : public OmDoubleSpinBox,
                             public om::event::MSTEventListener {
  Q_OBJECT;

 public:
  SizeThresholdSpinBox(GraphTools* d) : OmDoubleSpinBox(d, false), mParent(d) {
    setSingleStep(1);
    setMaximum(std::numeric_limits<double>::infinity());
    setDecimals(0);
    setInitialGUIThresholdValue();
  }

 private:
  GraphTools* const mParent;

  virtual void RefreshMSTEvent(om::event::MSTEvent*) {
    auto threshold = getCurVolThreshold();
    if (threshold) {
      setGUIvalue(*threshold);
    }
  }

  void setInitialGUIThresholdValue() {
    double t = 10000;

    auto threshold = getCurVolThreshold();
    if (threshold) {
      t = *threshold;
    }

    setGUIvalue(t);
  }

  void actUponValueChange(const double newThreshold);
  boost::optional<double> getCurVolThreshold();
};

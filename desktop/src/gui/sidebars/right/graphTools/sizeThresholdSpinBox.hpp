#pragma once
#include "precomp.h"

#include "actions/omActions.h"
#include "events/listeners.h"
#include "events/events.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "volume/omSegmentation.h"
#include "utility/segmentationDataWrapper.hpp"

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

  void actUponValueChange(const double newThreshold) {
    auto threshold = getCurVolThreshold();
    if (!threshold) {
      return;
    }

    if (qFuzzyCompare(*threshold, newThreshold)) {
      return;
    }

    OmActions::ChangeSizethreshold(mParent->GetSDW(), newThreshold);
  }

  boost::optional<double> getCurVolThreshold() {
    auto sdw = mParent->GetSDW();

    if (!sdw.IsSegmentationValid()) {
      return boost::optional<double>();
    }

    OmSegmentation& seg = *sdw.GetSegmentation();
    return boost::optional<double>(seg.GetSizeThreshold());
  }

};

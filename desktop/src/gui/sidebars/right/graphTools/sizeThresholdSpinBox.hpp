#pragma once

#include "actions/omActions.h"
#include "common/omDebug.h"
#include "events/details/omRefreshMSTthreshold.h"
#include "events/omEvents.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include <limits>

class SizeThresholdSpinBox : public OmDoubleSpinBox,
                             public OmRefreshMSTthresholdEventListener {
  Q_OBJECT public : SizeThresholdSpinBox(GraphTools* d)
                    : OmDoubleSpinBox(d, om::DONT_UPDATE_AS_TYPE), mParent(d) {
    setSingleStep(1);
    setMaximum(std::numeric_limits<double>::infinity());
    setDecimals(0);
    setInitialGUIThresholdValue();
  }

 private:
  GraphTools* const mParent;

  virtual void RefreshMSTEvent(OmRefreshMSTthresholdEvent*) {
    boost::optional<double> threshold = getCurVolThreshold();
    if (threshold) {
      setGUIvalue(*threshold);
    }
  }

  void setInitialGUIThresholdValue() {
    double t = 10000;

    boost::optional<double> threshold = getCurVolThreshold();
    if (threshold) {
      t = *threshold;
    }

    setGUIvalue(t);
  }

  void actUponValueChange(const double newThreshold) {
    boost::optional<double> threshold = getCurVolThreshold();
    if (!threshold) {
      return;
    }

    if (qFuzzyCompare(*threshold, newThreshold)) {
      return;
    }

    OmActions::ChangeSizethreshold(mParent->GetSDW(), newThreshold);
  }

  boost::optional<double> getCurVolThreshold() {
    SegmentationDataWrapper sdw = mParent->GetSDW();

    if (!sdw.IsSegmentationValid()) {
      return boost::optional<double>();
    }

    OmSegmentation& seg = sdw.GetSegmentation();
    return boost::optional<double>(seg.GetSizeThreshold());
  }
};

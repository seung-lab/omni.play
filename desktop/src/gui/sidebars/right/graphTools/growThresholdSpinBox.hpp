#pragma once

#include "actions/omActions.h"
#include "events/listeners.h"
#include "events/events.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

class GrowThresholdSpinBox
    : public OmDoubleSpinBox,
      public om::event::MSTEventListener {
Q_OBJECT
public:
    GrowThresholdSpinBox(GraphTools* d)
        : OmDoubleSpinBox(d, false)
        , mParent(d)
    {
        setSingleStep(0.002);
        setMaximum(1.0);
        setDecimals(3);
        setInitialGUIThresholdValue();
    }

private:
    GraphTools *const mParent;

    virtual void RefreshMSTEvent(om::event::MSTEvent*) {
        boost::optional<double> threshold = getCurVolThreshold();
        if (threshold) {
            setGUIvalue(*threshold);
        }
    }

    void setInitialGUIThresholdValue() {
        double t = 0.95;

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

        mParent->GetSDW().GetSegmentation()->SetGrowThreshold(newThreshold);
    }

    boost::optional<double> getCurVolThreshold() {
       SegmentationDataWrapper sdw = mParent->GetSDW();

       if (!sdw.IsSegmentationValid()) {
           return boost::optional<double>();
       }

       OmSegmentation* seg = sdw.GetSegmentation();
       return boost::optional<double>(seg->GetGrowThreshold());
    }
};

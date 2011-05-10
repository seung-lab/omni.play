#pragma once

#include "actions/omActions.h"
#include "common/omDebug.h"
#include "gui/sidebars/right/graphTools.h"
#include "gui/sidebars/right/thresholdGroup.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "events/omEvents.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

class ThresholdGroup : public OmDoubleSpinBox {
Q_OBJECT
public:
    ThresholdGroup(GraphTools * d)
        : OmDoubleSpinBox(d, om::DONT_UPDATE_AS_TYPE)
        , mParent(d)
    {
        setSingleStep(0.002);
        setMaximum(1.0);
        setDecimals(3);
        setInitialGUIThresholdValue();

        om::connect(this, SIGNAL(signalRefreshThreshold()),
                    this, SLOT(refreshThreshold()));
    }

    void RefreshThreshold(){
        signalRefreshThreshold();
    }

Q_SIGNALS:
    void signalRefreshThreshold();

private Q_SLOTS:

    void refreshThreshold()
    {
        boost::optional<double> threshold = getCurVolThreshold();
        if(threshold){
            setGUIvalue(*threshold);
        }
    }

private:
    GraphTools *const mParent;

    void setInitialGUIThresholdValue()
    {
        double t = 0.95;

        boost::optional<double> threshold = getCurVolThreshold();
        if(threshold){
            t = *threshold;
        }

        setGUIvalue(t);
    }

    void actUponValueChange(const double newThreshold)
    {
        boost::optional<double> threshold = getCurVolThreshold();
        if(!threshold){
            return;
        }

        if(qFuzzyCompare(*threshold, newThreshold)){
            return;
        }

        OmActions::ChangeMSTthreshold(mParent->GetSDW(), newThreshold);
    }

    boost::optional<double> getCurVolThreshold()
    {
       SegmentationDataWrapper sdw = mParent->GetSDW();

       if(!sdw.IsSegmentationValid()){
           return boost::optional<double>();
       }

       OmSegmentation& seg = sdw.GetSegmentation();
       return boost::optional<double>(seg.GetDendThreshold());
    }
};


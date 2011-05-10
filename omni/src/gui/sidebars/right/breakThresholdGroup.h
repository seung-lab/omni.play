#pragma once

#include "actions/omActions.h"
#include "common/omDebug.h"
#include "gui/sidebars/right/graphTools.h"
#include "gui/sidebars/right/thresholdGroup.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "events/omEvents.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "viewGroup/omViewGroupState.h"
#include "system/cache/omCacheManager.h"

class BreakThresholdGroup : public OmDoubleSpinBox {
Q_OBJECT
public:
    BreakThresholdGroup(GraphTools* d, OmViewGroupState* vgs)
        : OmDoubleSpinBox(d, om::UPDATE_AS_TYPE)
        , vgs_(vgs)
    {
        setSingleStep(0.002);
        setMaximum(1.0);
        setDecimals(3);
        setInitialGUIThresholdValue();
    }

private:
    OmViewGroupState *const vgs_;

    void setInitialGUIThresholdValue()
    {
        static const double threshold = 1.;

        setGUIvalue(threshold);

        vgs_->setBreakThreshold(threshold);
    }

    void actUponValueChange(const double threshold)
    {
        vgs_->setBreakThreshold(threshold);

        // todo: don't use same freshness as normal tiles
        OmCacheManager::TouchFreshness();
        OmEvents::Redraw2d();
        OmEvents::Redraw3d();
    }
};


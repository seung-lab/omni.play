#ifndef BREAK_THRESHOLD_GROUP_H
#define BREAK_THRESHOLD_GROUP_H

#include "actions/omActions.h"
#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "events/omEvents.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "viewGroup/omViewGroupState.h"
#include "system/cache/omCacheManager.h"

class BreakThresholdGroup : public OmDoubleSpinBox {
Q_OBJECT
public:
    BreakThresholdGroup(GraphTools * d)
        : OmDoubleSpinBox(d, om::UPDATE_AS_TYPE)
        , mParent(d)
    {
        setSingleStep(0.002);
        setMaximum(1.0);
        setDecimals(3);
        setInitialGUIThresholdValue();
    }

private:
    GraphTools *const mParent;

    void setInitialGUIThresholdValue()
    {
        static const double threshold = 1.;

        setGUIvalue(threshold);

        OmViewGroupState * vgs = mParent->getViewGroupState();
        vgs->setBreakThreshold(threshold);
    }

    void actUponValueChange( const double threshold )
    {
        OmViewGroupState * vgs = mParent->getViewGroupState();
        vgs->setBreakThreshold(threshold);

        // todo: don't use same freshness as normal tiles
        OmCacheManager::TouchFreshness();
        OmEvents::Redraw2d();
        OmEvents::Redraw3d();
    }
};

#endif

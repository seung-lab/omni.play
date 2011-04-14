#ifndef CONTRAST_SPIN_BOX_HPP
#define CONTRAST_SPIN_BOX_HPP

#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/displayTools/displayTools.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "tiles/cache/omTileCache.h"
#include "tiles/omChannelTileFilter.hpp"
#include "viewGroup/omViewGroupState.h"

#include <limits>

class ContrastSpinBox : public OmDoubleSpinBox {
    Q_OBJECT

    public:
    ContrastSpinBox(DisplayTools* d)
        : OmDoubleSpinBox(d, om::UPDATE_AS_TYPE)
        , mParent(d)
    {
        setSingleStep(0.05);
        setRange(-5, 5);
        setInitialGUIThresholdValue();
    }

private:
    DisplayTools *const mParent;

    void actUponValueChange(const double threshold)
    {
        OmChannelTileFilter::SetContrastValue(threshold);
        OmTileCache::ClearChannel();
        OmEvents::Redraw2dBlocking();
    }

    void setInitialGUIThresholdValue()
    {
        setValue(OmChannelTileFilter::GetContrastValue());
    }

    OmViewGroupState* vgs() const {
        return mParent->getViewGroupState();
    }
};

#endif

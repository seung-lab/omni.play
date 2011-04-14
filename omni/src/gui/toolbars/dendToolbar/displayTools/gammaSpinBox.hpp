#ifndef GAMMA_SPIN_BOX_HPP
#define GAMMA_SPIN_BOX_HPP

#include "gui/toolbars/dendToolbar/displayTools/displayTools.h"
#include "gui/widgets/omDoubleSpinBox.hpp"
#include "tiles/cache/omTileCache.h"
#include "tiles/omChannelTileFilter.hpp"

class GammaSpinBox : public OmDoubleSpinBox {
Q_OBJECT

public:
    GammaSpinBox(DisplayTools* d)
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
        OmChannelTileFilter::SetGamma(threshold);
        OmTileCache::ClearChannel();
        OmEvents::Redraw2dBlocking();
    }

    void setInitialGUIThresholdValue(){
        setValue(OmChannelTileFilter::GetGamma());
    }
};

#endif

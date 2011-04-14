#ifndef BRIGHTNESS_SPIN_BOX_HPP
#define BRIGHTNESS_SPIN_BOX_HPP

#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/displayTools/displayTools.h"
#include "gui/widgets/omIntSpinBox.hpp"
#include "tiles/cache/omTileCache.h"
#include "tiles/omChannelTileFilter.hpp"

class BrightnessSpinBox : public OmIntSpinBox {
    Q_OBJECT

    public:
    BrightnessSpinBox(DisplayTools* d)
        : OmIntSpinBox(d, om::UPDATE_AS_TYPE)
        , mParent(d)
    {
        setSingleStep(1);
        setRange(-255, 255);
        setInitialGUIThresholdValue();
    }

private:
    DisplayTools *const mParent;

    void actUponSpinboxChange(const int val)
    {
        OmChannelTileFilter::SetBrightnessShift(val);
        OmTileCache::ClearChannel();
        OmEvents::Redraw2dBlocking();
    }

    void setInitialGUIThresholdValue(){
        setValue(OmChannelTileFilter::GetBrightnessShift());
    }
};

#endif

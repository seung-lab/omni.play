#pragma once

#include "common/omDebug.h"
#include "gui/widgets/omIntSpinBox.hpp"
#include "tiles/cache/omTileCache.h"
#include "tiles/omChannelTileFilter.hpp"

class BrightnessSpinBox : public OmIntSpinBox {
Q_OBJECT

public:
    BrightnessSpinBox(QWidget* d)
        : OmIntSpinBox(d, om::UPDATE_AS_TYPE)
    {
        setSingleStep(1);
        setRange(-255, 255);
        setInitialGUIThresholdValue();
    }

    QString Label() const {
        return "Brightness";
    }

private:

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


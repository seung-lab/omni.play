#pragma once

#include "gui/widgets/omIntSpinBox.hpp"
#include "events/omEvents.h"
#include "viewGroup/omViewGroupState.h"

#include <limits>

class Dust3DThresholdGroup : public OmIntSpinBox {
Q_OBJECT

public:
    Dust3DThresholdGroup(QWidget* parent, OmViewGroupState* vgs)
        : OmIntSpinBox(parent, true)
        , vgs_(vgs)
    {
        setSingleStep(5);
        setMaximum(std::numeric_limits<int32_t>::max());
        setInitialGUIThresholdValue();
    }

private:
    OmViewGroupState *const vgs_;

    void actUponValueChange(const int threshold)
    {
        vgs_->setDustThreshold(threshold);
        OmEvents::Redraw3d();
    }

    void setInitialGUIThresholdValue()
    {
        int dThreshold = vgs_->getDustThreshold();
        setValue(dThreshold);
    }
};


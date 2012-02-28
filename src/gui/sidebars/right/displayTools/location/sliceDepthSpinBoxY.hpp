#pragma once

#include "gui/sidebars/right/displayTools/location/sliceDepthSpinBoxBase.hpp"

class SliceDepthSpinBoxY : public SliceDepthSpinBoxBase {
Q_OBJECT

public:
    SliceDepthSpinBoxY(QWidget* d, OmViewGroupState* vgs)
        : SliceDepthSpinBoxBase(d, vgs)
    {}

    virtual QString Label() const {
        return "Y";
    }

private:
    virtual ViewType viewType() const {
        return XZ_VIEW;
    }
};


#pragma once

#include "gui/sidebars/right/displayTools/location/sliceDepthSpinBoxBase.hpp"

class SliceDepthSpinBoxX : public SliceDepthSpinBoxBase {
Q_OBJECT

public:
    SliceDepthSpinBoxX(QWidget* d, OmViewGroupState* vgs)
        : SliceDepthSpinBoxBase(d, vgs)
    {}

    virtual QString Label() const {
        return "X";
    }

private:
    virtual ViewType viewType() const {
        return ZY_VIEW;
    }
};


#pragma once

#include "gui/sidebars/right/displayTools/sliceDepthSpinBoxBase.hpp"

class SliceDepthSpinBoxZ : public SliceDepthSpinBoxBase {
Q_OBJECT

public:
    SliceDepthSpinBoxZ(QWidget* d, OmViewGroupState* vgs)
        : SliceDepthSpinBoxBase(d, vgs)
    {}

private:
    virtual ViewType viewType() const {
        return XY_VIEW;
    }
};


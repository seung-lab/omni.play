#pragma once

#include "gui/sidebars/right/displayTools/sliceDepthSpinBoxBase.hpp"

class SliceDepthSpinBoxY : public SliceDepthSpinBoxBase {
Q_OBJECT

public:
    SliceDepthSpinBoxY(QWidget* d, OmViewGroupState* vgs)
        : SliceDepthSpinBoxBase(d, vgs)
    {}

private:
    virtual ViewType viewType() const {
        return XZ_VIEW;
    }
};


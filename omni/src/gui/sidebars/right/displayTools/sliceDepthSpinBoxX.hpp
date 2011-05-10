#pragma once

#include "gui/sidebars/right/displayTools/sliceDepthSpinBoxBase.hpp"

class SliceDepthSpinBoxX : public SliceDepthSpinBoxBase {
Q_OBJECT

public:
    SliceDepthSpinBoxX(QWidget* d, OmViewGroupState* vgs)
        : SliceDepthSpinBoxBase(d, vgs)
    {}

private:
    virtual ViewType viewType() const {
        return ZY_VIEW;
    }
};


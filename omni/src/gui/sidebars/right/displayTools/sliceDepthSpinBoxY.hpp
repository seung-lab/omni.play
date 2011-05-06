#ifndef SLICE_DEPTH_SPIN_BOX_Y_HPP
#define SLICE_DEPTH_SPIN_BOX_Y_HPP

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

#endif

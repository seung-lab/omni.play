#ifndef SLICE_DEPTH_SPIN_BOX_X_HPP
#define SLICE_DEPTH_SPIN_BOX_X_HPP

#include "gui/toolbars/dendToolbar/displayTools/sliceDepthSpinBoxBase.hpp"

class SliceDepthSpinBoxX : public SliceDepthSpinBoxBase {
Q_OBJECT

public:
    SliceDepthSpinBoxX(DisplayTools* d)
        : SliceDepthSpinBoxBase(d)
    {}

private:
    virtual ViewType viewType() const {
        return YZ_VIEW;
    }
};

#endif

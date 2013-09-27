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
    virtual om::common::ViewType viewType() const {
        return om::common::XZ_VIEW;
    }
};


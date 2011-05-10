#pragma once

#include "common/omDebug.h"
#include "events/details/omViewEvent.h"
#include "gui/widgets/omIntSpinBox.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"

#include <limits>

class SliceDepthSpinBoxBase : public OmIntSpinBox,
                              public OmViewEventListener {
Q_OBJECT

public:
    SliceDepthSpinBoxBase(QWidget* d, OmViewGroupState* vgs)
        : OmIntSpinBox(d, om::UPDATE_AS_TYPE)
        , vgs_(vgs)
    {
        setValue(0);
        setSingleStep(1);
        setMaximum(std::numeric_limits<int32_t>::max());
    }

    QSize sizeHint () const {
        return QSize(50, height());
    }

private:
    OmViewGroupState *const vgs_;

    virtual ViewType viewType() const = 0;

    void actUponSpinboxChange(const int depth)
    {
        if(NULL == vgs()){
            return;
        }

        vgs()->SetScaledSliceDepth(viewType(), depth);
        OmEvents::Redraw2d();
    }

    void update()
    {
        blockSignals(true);

        const int depth = vgs()->GetScaledSliceDepth(viewType());
        setValue(depth);

        blockSignals(false);
    }

    inline OmViewGroupView2dState* vgs() const {
        return vgs_->View2dState();
    }

    // OmViewEventListener
    void ViewBoxChangeEvent(){
        update();
    }
    void ViewCenterChangeEvent(){
        update();
    }
    void ViewPosChangeEvent(){}
    void ViewRedrawEvent(){}
    void ViewBlockingRedrawEvent(){}
};


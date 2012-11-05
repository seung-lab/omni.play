#pragma once

#include "common/omDebug.h"
#include "events/details/omViewEvent.h"
#include "gui/widgets/omIntSpinBox.hpp"
#include "utility/dataWrappers.h"
#include "view2d/omView2dConverters.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "volume/omMipVolume.h"

#include <limits>

class SliceDepthSpinBoxBase : public OmIntSpinBox,
                              public OmViewEventListener {
Q_OBJECT

public:
    SliceDepthSpinBoxBase(QWidget* d, OmViewGroupState* vgs)
        : OmIntSpinBox(d, om::common::UPDATE_AS_TYPE)
        , vgs_(vgs)
    {
        setValue(0);
        setSingleStep(1);
        setMaximum(std::numeric_limits<int32_t>::max());
    }

    QSize sizeHint () const {
        return QSize(50, height());
    }

    virtual QString Label() const = 0;

private:
    OmViewGroupState *const vgs_;

    virtual om::common::ViewType viewType() const = 0;

    OmMipVolume* getVol()
    {
        {
            const ChannelDataWrapper cdw = vgs_->Channel();
            if(cdw.IsValidWrapper()){
                return cdw.GetChannelPtr();
            }
        }

        {
            const SegmentationDataWrapper sdw = vgs_->Segmentation();
            if(sdw.IsValidWrapper()){
                return sdw.GetSegmentationPtr();
            }
        }

        return NULL;
    }

    void actUponValueChange(const int depth)
    {
        if(NULL == vg2ds()){
            return;
        }

        vg2ds()->SetScaledSliceDepth(viewType(), depth);
        OmEvents::Redraw2d();
        OmEvents::ViewCenterChanged();
        OmEvents::View3dRecenter();
    }

    void update()
    {
        blockSignals(true);

        const int depth = vg2ds()->GetScaledSliceDepth(viewType());

        setValue(depth);

        blockSignals(false);
    }

    inline OmViewGroupView2dState* vg2ds() const {
        return vgs_->View2dState();
    }

    // OmViewEventListener
    void ViewBoxChangeEvent(){
        update();
    }
    void ViewCenterChangeEvent(){
        update();
    }
    void CoordSystemChangeEvent() {
        update();
    }
    void ViewPosChangeEvent(){}
    void ViewRedrawEvent(){}
    void ViewBlockingRedrawEvent(){}
};


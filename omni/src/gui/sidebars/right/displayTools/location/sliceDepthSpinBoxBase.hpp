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

    virtual QString Label() const = 0;

private:
    OmViewGroupState *const vgs_;

    virtual ViewType viewType() const = 0;

    // TODO: remove: hack for abs coords
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

    // TODO: remove: hack for abs coords
    int getOffset()
    {
        OmMipVolume* vol = getVol();

        if(vol)
        {
            const Vector3i absOffset = vol->Coords().GetAbsOffset();
            return OmView2dConverters::GetViewTypeDepth(absOffset, viewType());
        }

        return 0;
    }

    void actUponSpinboxChange(const int depth)
    {
        if(NULL == vg2ds()){
            return;
        }

        // TODO: refactor abs coords into viewgroupstate (i.e. remove hack)
        const int offset = getOffset();

        vg2ds()->SetScaledSliceDepth(viewType(), depth - offset);
        OmEvents::Redraw2d();
    }

    void update()
    {
        blockSignals(true);

        const int depth = vg2ds()->GetScaledSliceDepth(viewType());

        // TODO: remove: hack for abs coords
        const int offset = getOffset();

        setValue(depth + offset);

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
    void ViewPosChangeEvent(){}
    void ViewRedrawEvent(){}
    void ViewBlockingRedrawEvent(){}
};


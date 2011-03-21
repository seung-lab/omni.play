#ifndef OM_VIEW2D_EVENTS_HPP
#define OM_VIEW2D_EVENTS_HPP

#include "events/details/omSegmentEvent.h"
#include "events/details/omToolModeEvent.h"
#include "events/details/omViewEvent.h"
#include "gui/widgets/omCursors.h"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"

// deal w/ Omni events
class OmView2dEvents
    : public OmSegmentEventListener,
      public OmViewEventListener,
      public OmToolModeEventListener
{
private:
    OmView2d *const v2d_;
    OmView2dState *const state_;

public:
    OmView2dEvents(OmView2d* v2d, OmView2dState* state)
        : v2d_(v2d)
        , state_(state)
    {}

    void SegmentModificationEvent(OmSegmentEvent*){
        v2d_->myUpdate();
    }

    void ViewBoxChangeEvent(){
        v2d_->myUpdate();
    }
    void ViewPosChangeEvent(){
        v2d_->myUpdate();
    }
    void ViewCenterChangeEvent()
    {
        state_->ChangeViewCenter();
        v2d_->myUpdate();
        OmEvents::Redraw3d();
    }
    void ViewRedrawEvent(){
        v2d_->myUpdate();
    }

    void ToolModeChangeEvent(){
        OmCursors::setToolCursor(v2d_);
    }
};
#endif

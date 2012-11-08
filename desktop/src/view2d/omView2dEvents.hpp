#pragma once

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
        v2d_->Redraw();
    }

    void SegmentGUIlistEvent(OmSegmentEvent*)
    {}

    void SegmentSelectedEvent(OmSegmentEvent*)
    {}

    void ViewBoxChangeEvent()
    {
        state_->UpdateTransformationMatrices();
        v2d_->Redraw();
    }

    void ViewPosChangeEvent()
    {
        state_->UpdateTransformationMatrices();
        v2d_->Redraw();
    }

    void ViewCenterChangeEvent()
    {
        state_->UpdateTransformationMatrices();
        state_->ChangeViewCenter();
        v2d_->Redraw();
        OmEvents::Redraw3d();
    }

    void ViewRedrawEvent()
    {
        state_->UpdateTransformationMatrices();
        v2d_->Redraw();
    }

    void ViewBlockingRedrawEvent()
    {
        state_->UpdateTransformationMatrices();
        v2d_->RedrawBlocking();
    }

    void ToolModeChangeEvent()
    {
        OmCursors::setToolCursor(v2d_);
    }
    
    void CoordSystemChangeEvent() {}
};

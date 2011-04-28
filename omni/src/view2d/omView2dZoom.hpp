#ifndef OM_VIEW2D_ZOOM_HPP
#define OM_VIEW2D_ZOOM_HPP

#include "view2d/omView2dState.hpp"

#include <QMouseEvent>

class OmView2dZoom{
private:
    OmView2dState *const state_;
    OmZoomLevel *const zoomLevel_;

public:
    OmView2dZoom(OmView2dState* state)
        : state_(state)
        , zoomLevel_(state->ZoomLevel())
    {}

    void MouseLeftButtonClick(QMouseEvent* event)
    {
        const bool zoomOut = event->modifiers() & Qt::ControlModifier;

        if (zoomOut) {
            doMouseZoom(-15);
        } else {
            doMouseZoom(15);
        }
    }

    void MouseWheelZoom(const int numSteps){
        doMouseZoom(numSteps);
    }

    void KeyboardZoomIn(){
        doMouseZoom(1);
    }

    void KeyboardZoomOut(){
        doMouseZoom(-1);
    }

private:
    void doMouseZoom(const int numSteps)
    {
        zoomLevel_->MouseWheelZoom(numSteps, state_->IsLevelLocked());

        OmEvents::ViewPosChanged();
        state_->SetViewSliceOnPan();
        OmEvents::ViewCenterChanged();
    }
};

#endif

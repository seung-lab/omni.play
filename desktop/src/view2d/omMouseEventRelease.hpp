#pragma once

#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"

class OmMouseEventRelease{
private:
    OmView2d *const v2d_;
    OmView2dState *const state_;

    om::globalCoord dataClickPoint_;

public:
    OmMouseEventRelease(OmView2d* v2d, OmView2dState* state)
        : v2d_(v2d)
        , state_(state)
    {}

    void Release(QMouseEvent* event)
    {
        setState(event);

        state_->setScribbling(false);
        state_->SetLastDataPoint(dataClickPoint_);

        switch (OmStateManager::GetToolMode())
        {
        case om::tool::PAINT:
        case om::tool::ERASE:
        case om::tool::SELECT:
        case om::tool::LANDMARK:
            state_->OverrideToolModeForPan(false);
            break;
        default:
            break;
        }
    }

private:
    void setState(QMouseEvent* event){
        om::screenCoord clicked(Vector2i(event->x(), event->y()),state_);
        dataClickPoint_ = clicked.toGlobalCoord();
    }
};


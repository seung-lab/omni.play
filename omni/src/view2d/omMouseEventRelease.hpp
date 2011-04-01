#ifndef OM_MOUSE_EVENT_RELEASE_HPP
#define OM_MOUSE_EVENT_RELEASE_HPP

#include "gui/segmentLists/elementListBox.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"

class OmMouseEventRelease{
private:
    OmView2d *const v2d_;
    OmView2dState *const state_;

    DataCoord dataClickPoint_;

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

        switch (OmStateManager::GetToolMode()) {
        case om::tool::SPLIT:
        case om::tool::CUT:
        case om::tool::CROSSHAIR:
        case om::tool::PAN:
        case om::tool::ZOOM:
        case om::tool::FILL:
        case om::tool::PAINT:
        case om::tool::ERASE:
            break;

        case om::tool::SELECT:
            ElementListBox::PopulateLists();
            state_->AmPanningInSelectMode(false);
            break;
        }
    }

private:
    void setState(QMouseEvent* event)
    {
        dataClickPoint_ = state_->ComputeMouseClickPointDataCoord(event);
    }
};

#endif

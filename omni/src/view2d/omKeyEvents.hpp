#ifndef OM_KEY_EVENTS_HPP
#define OM_KEY_EVENTS_HPP

#include "segment/omSegmentCenter.hpp"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentUtils.hpp"
#include "system/cache/omCacheManager.h"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "view2d/omView2dZoom.hpp"
#include "viewGroup/omBrushSize.hpp"

#include <QKeyEvent>

class OmKeyEvents{
private:
    OmView2d *const v2d_;
    OmView2dState *const state_;

public:
    OmKeyEvents(OmView2d* v2d, OmView2dState* state)
        : v2d_(v2d)
        , state_(state)
    {}

    bool Press(QKeyEvent* event)
    {
        switch (event->key()) {

        case Qt::Key_J:
            Join();
            break;

        case Qt::Key_L:
            state_->ToggleLevelLock();
            v2d_->Redraw();
            break;

        case Qt::Key_Escape:
            v2d_->ResetWidget();
            OmCacheManager::ClearCacheContents();
            break;

        case Qt::Key_Minus:
            v2d_->Zoom()->KeyboardZoomOut();
            break;
        case Qt::Key_Equal:
            v2d_->Zoom()->KeyboardZoomIn();
            break;

        case Qt::Key_Right:
            state_->Shift(om::RIGHT);
            break;
        case Qt::Key_Left:
            state_->Shift(om::LEFT);
            break;
        case Qt::Key_Up:
            state_->Shift(om::UP);
            break;
        case Qt::Key_Down:
            state_->Shift(om::DOWN);
            break;

        case Qt::Key_C:
            OmSegmentCenter::CenterSegment(state_->getViewGroupState(),
                                           state_->GetSDW());
            break;

        case Qt::Key_W:
        case Qt::Key_PageUp:
            state_->MoveUpStackCloserToViewer();
            OmEvents::ViewCenterChanged();
            break;

        case Qt::Key_S:
        case Qt::Key_E:
        case Qt::Key_PageDown:
            state_->MoveDownStackFartherFromViewer();
            OmEvents::ViewCenterChanged();
            break;

        case Qt::Key_Tab:
        {
            const bool control = event->modifiers() & Qt::ControlModifier;
            if(control){
                v2d_->ShowComplimentaryDock();
            }
        }
        break;

        default:
            return false;
        }

        return true; // we handled event
    }

private:
    void Join()
    {
        OmActions::JoinSegments(state_->GetSegmentationID());
        OmEvents::Redraw2d();
    }
};

#endif

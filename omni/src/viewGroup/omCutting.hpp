#pragma once

#include "system/omStateManager.h"
#include "events/omEvents.h"
#include "gui/toolbars/toolbarManager.h"

class OmCutting {
private:
    bool showCut_;
    ToolBarManager* toolBarManager_;

public:
    OmCutting()
        : showCut_(false)
        , toolBarManager_(NULL)
    {}

    void SetToolBarManager(ToolBarManager* tbm){
        toolBarManager_ = tbm;
    }

    void EnterCutMode()
    {
        showCut_ = true;

        OmStateManager::SetToolModeAndSendEvent(om::tool::CUT);
        OmEvents::Redraw3d();
        OmEvents::Redraw2d();
    }

    void ExitCutModeFixButton()
    {
        if(showCut_){
            toolBarManager_->SetCuttingOff();
            ExitCutMode();
        }
    }

    void ExitCutMode()
    {
        showCut_ = false;

        OmStateManager::SetOldToolModeAndSendEvent();
        OmEvents::Redraw3d();
        OmEvents::Redraw2d();
    }
};


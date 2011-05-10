#pragma once

#include "events/details/omSegmentEvent.h"
#include "events/details/omToolModeEvent.h"
#include "gui/brushToolbox/brushToolboxImpl.h"
#include "system/omStateManager.h"

class BrushToolbox : public OmToolModeEventListener,
                     public OmSegmentEventListener {
private:
    QWidget *const parent_;
    boost::scoped_ptr<BrushToolboxImpl> dock_;

    bool alreadyPoppedMenu_;

    void showForFirstTime()
    {
        if(alreadyPoppedMenu_){
            return;
        }

        Show();
    }

    void SegmentModificationEvent(OmSegmentEvent*){}
    void SegmentGUIlistEvent(OmSegmentEvent*){}
    void SegmentSelectedEvent(OmSegmentEvent*){
        showForFirstTime();
    }

    void ToolModeChangeEvent()
    {
        if(alreadyPoppedMenu_){
            return;
        }

        switch(OmStateManager::GetToolMode()){
        case om::tool::ERASE:
        case om::tool::FILL:
        case om::tool::PAINT:
            Show();
            break;
        default:
            break;
        }
    }

public:
    BrushToolbox(QWidget* parent)
        : parent_(parent)
        , dock_(new BrushToolboxImpl(parent_))
        , alreadyPoppedMenu_(false)
    {}

    void Show()
    {
        dock_->show();
        alreadyPoppedMenu_ = true;
    }

    void Hide(){
        dock_->hide();
    }
};


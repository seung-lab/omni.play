#ifndef BRUSH_TOOLBOX_HPP
#define BRUSH_TOOLBOX_HPP

#include "system/omStateManager.h"
#include "events/details/omToolModeEvent.h"
#include "gui/brushToolbox/brushToolboxImpl.hpp"

class BrushToolbox : public OmToolModeEventListener {
private:
    QWidget *const mainWindow_;

    boost::scoped_ptr<BrushToolboxImpl> dock_;

    void makeToolbox()
    {
        dock_.reset(new BrushToolboxImpl(mainWindow_));

        dock_->show();
        dock_->raise();
    }

    void showPaintBox()
    {
        if(dock_)
        {
            // dock_->show();
            return;
        }

        // makeToolbox();
    }

    void showEraseBox()
    {
        if(dock_)
        {
            // dock_->show();
            return;
        }

        // makeToolbox();
    }

    void hideBox()
    {
        if(dock_)
        {
            dock_->hide();
            return;
        }
    }

public:
    BrushToolbox(QWidget* mainWindow)
        : mainWindow_(mainWindow)
    {}

    void ToolModeChangeEvent()
    {
        const om::tool::mode tool = OmStateManager::GetToolMode();

        switch(tool){
        case om::tool::PAINT:
            showPaintBox();
            break;
        case om::tool::ERASE:
            showEraseBox();
            break;
        default:
            hideBox();
        }
    }
};

#endif

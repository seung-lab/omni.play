#pragma once

#include "gui/sidebars/right/dendToolbar.h"
#include "gui/toolbars/mainToolbar/mainToolbar.h"

class OmViewGroupState;

namespace om {
namespace sidebars {

class right {
private:
    DendToolBar* dendToolBar_;

public:
    right(MainWindow* mainWindow, OmViewGroupState* vgs)
    {
        dendToolBar_ = new DendToolBar(mainWindow, vgs);

        QDockWidget* dock = new QDockWidget("Tools", mainWindow);
        dock->setAllowedAreas(Qt::RightDockWidgetArea);
        dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dock->setWidget(dendToolBar_);

        dendToolBar_->setParent(dock);

        mainWindow->addDockWidget(Qt::RightDockWidgetArea, dock);
    }

    void SetSplittingOff(){
        dendToolBar_->SetSplittingOff();
    }

    void SetCuttingOff(){
        dendToolBar_->SetCuttingOff();
    }
};

} // namespace sidebars
} // namespace om

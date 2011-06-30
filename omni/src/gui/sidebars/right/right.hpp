#pragma once

#include "gui/sidebars/right/rightImpl.h"
#include "gui/toolbars/mainToolbar/mainToolbar.h"

class OmViewGroupState;

namespace om {
namespace sidebars {

class right {
private:
    rightImpl* impl_;

public:
    right(MainWindow* mainWindow, OmViewGroupState* vgs)
    {
        impl_ = new rightImpl(mainWindow, vgs);

        QDockWidget* dock = new QDockWidget("Tools", mainWindow);
        dock->setAllowedAreas(Qt::RightDockWidgetArea);
        dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
        dock->setWidget(impl_);

        impl_->setParent(dock);

        mainWindow->addDockWidget(Qt::RightDockWidgetArea, dock);
    }

    void SetSplittingOff(){
        impl_->SetSplittingOff();
    }
};

} // namespace sidebars
} // namespace om

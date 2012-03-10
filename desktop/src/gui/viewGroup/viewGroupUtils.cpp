#include "gui/viewGroup/viewGroupUtils.hpp"
#include "gui/mainWindow/mainWindow.h"
#include "system/omConnect.hpp"
#include "viewGroup/omViewGroupState.h"
#include "view2d/omView2d.h"

int om::gui::ViewGroupUtils::getID(){
    return vgs_->GetID();
}

QList<QDockWidget*> om::gui::ViewGroupUtils::findDockWidgets(const QString& name){
    return mainWindow_->findChildren<QDockWidget*>(name);
}

QList<QDockWidget*> om::gui::ViewGroupUtils::findDockWidgets(const QRegExp& regExp){
    return mainWindow_->findChildren<QDockWidget*>(regExp);
}

void om::gui::ViewGroupUtils::connectVisibilityChange(OmView2d* w, QDockWidget* dock)
{
    // used to let tile cache know when view2d visibilty changes
    om::connect(dock, SIGNAL(visibilityChanged(bool)),
                w, SLOT(dockVisibilityChanged(bool)));
}

void om::gui::ViewGroupUtils::setComplimentaryDockWidget(OmView2d* w,
                                                         QDockWidget* dock,
                                                         QDockWidget* complimentaryDock)
{
    // used for cntrl+tab key in view2d

    if(!complimentaryDock){
        return;
    }

    w->SetComplimentaryDockWidget(complimentaryDock);

    QWidget* compWidget = complimentaryDock->widget();

    if(compWidget)
    {
        OmView2d* v2d = static_cast<OmView2d*>(compWidget);
        v2d->SetComplimentaryDockWidget(dock);
    }
}

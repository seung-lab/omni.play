#pragma once

#include "gui/menubar.h"
#include "gui/viewGroup/viewGroupUtils.hpp"

namespace om {
namespace gui {

class ViewGroupMainWindowUtils {
private:
    MainWindow *const mainWindow_;
    OmViewGroupState *const vgs_;
    ViewGroupUtils *const utils_;

    static const ViewType upperLeft_ = XY_VIEW;
    static const ViewType upperRight_ = ZY_VIEW;
    static const ViewType lowerLeft_ = XZ_VIEW;

public:
    ViewGroupMainWindowUtils(MainWindow* mainWindow, OmViewGroupState* vgs, ViewGroupUtils* utils)
        : mainWindow_(mainWindow)
        , vgs_(vgs)
        , utils_(utils)
    {}

    void InsertDockIntoGroup2View(ViewGroupWidgetInfo& vgw, const ViewType viewType)
    {
        if(0 == getNumDockWidgets())
        {
            QDockWidget* dock = makeDockWidget(vgw);
            mainWindow_->addDockWidget(Qt::BottomDockWidgetArea, dock);
            return;
        }

        QDockWidget* dockToSplit = NULL;

        if(utils_->doesDockWidgetExist(CHANNEL, viewType)){
            dockToSplit = utils_->getDockWidget(CHANNEL, viewType);

        } else if(utils_->doesDockWidgetExist(SEGMENTATION, viewType)){
            dockToSplit = utils_->getDockWidget(SEGMENTATION, viewType);

        } else if(utils_->doesDockWidgetExist(AFFINITY, viewType)){
            dockToSplit = utils_->getDockWidget(AFFINITY, viewType);
            
        } else {
            throw OmArgException("don't know where to put dock");
        }

        InsertBySplitting(vgw, dockToSplit);
    }

    DockWidgetPair InsertDockIntoGroup4View(ViewGroupWidgetInfo& vgw)
    {
        QDockWidget* dock = NULL;
        QDockWidget* dockToTabify = NULL;

        if(0 == getNumDockWidgets())
        {
            dock = makeDockWidget(vgw);
            mainWindow_->addDockWidget(Qt::BottomDockWidgetArea, dock);

        } else {
            dockToTabify = chooseDockToTabify(vgw);

            if(dockToTabify){
                dock = insertByTabbing(vgw, dockToTabify);

            } else {
                dock = InsertBySplitting(vgw, chooseDockToSplit(vgw));
            }
        }

        DockWidgetPair ret = { dock, dockToTabify };

        return ret;
    }

    QDockWidget* InsertBySplitting(ViewGroupWidgetInfo& vgw, QDockWidget* biggest)
    {
        QList<QDockWidget*> tabified = mainWindow_->tabifiedDockWidgets(biggest);

        if(!tabified.empty())
        {
            Q_FOREACH(QDockWidget* widget, tabified){
                mainWindow_->removeDockWidget(widget);
            }
        }

        const Qt::Orientation dir = vgw.Dir();

        QDockWidget* dock = makeDockWidget(vgw);

        //debug(viewGroup, "\t inserting %s by splitting...\n",
        //qPrintable(dock->objectName()));

        mainWindow_->splitDockWidget(biggest, dock, dir);

        if(!tabified.empty())
        {
            Q_FOREACH(QDockWidget* dwidget, tabified)
            {
                dwidget->show();
                mainWindow_->tabifyDockWidget(biggest, dwidget);
            }
        }

        return dock;
    }

private:
    int getNumDockWidgets()
    {
        QList<QDockWidget*> widgets = utils_->getAllDockWidgets();
        return widgets.size();
    }

    QDockWidget* insertByTabbing(ViewGroupWidgetInfo& vgw, QDockWidget* widgetToTabify)
    {
        QDockWidget* dock = makeDockWidget(vgw);
        //debug(viewGroup, "\t inserting %s by tabbing...\n",
        //qPrintable(dock->objectName()));
        mainWindow_->tabifyDockWidget(widgetToTabify, dock);

        return dock;
    }

    QDockWidget* makeDockWidget(ViewGroupWidgetInfo& vgw)
    {
        QDockWidget* dock = new QDockWidget(vgw.name, mainWindow_);
        vgw.widget->setParent(dock);

        dock->setObjectName(utils_->makeObjectName(vgw));
        dock->setWidget(vgw.widget);
        dock->setAllowedAreas(Qt::AllDockWidgetAreas);
        dock->setAttribute(Qt::WA_DeleteOnClose);

        // mainWindow_->mMenuBar->GetWindowMenu()->addAction(dock->toggleViewAction());

        return dock;
    }

    QDockWidget* chooseDockToSplit(ViewGroupWidgetInfo& vgw)
    {
        QDockWidget* dock = utils_->getBiggestDockWidget();

        if(VIEW2D_CHAN == vgw.widgetType)
        {
            if(upperLeft_ == vgw.viewType){

            } else if(upperRight_ == vgw.viewType){
                if(utils_->doesDockWidgetExist(CHANNEL, upperLeft_)){
                    dock = utils_->getDockWidget(CHANNEL, upperLeft_);
                }
            } else {
                if(utils_->doesDockWidgetExist(CHANNEL, upperLeft_)){
                    dock = utils_->getDockWidget(CHANNEL, upperLeft_);
                }
                vgw.Dir(Qt::Vertical);
            }

        } else if(VIEW2D_SEG == vgw.widgetType)
        {
            if(upperLeft_ == vgw.viewType){

            } else if(upperRight_ == vgw.viewType){
                if(utils_->doesDockWidgetExist(SEGMENTATION, upperLeft_)){
                    dock = utils_->getDockWidget(SEGMENTATION, upperLeft_);
                }
            } else {
                if(utils_->doesDockWidgetExist(SEGMENTATION, upperLeft_)){
                    dock = utils_->getDockWidget(SEGMENTATION, upperLeft_);
                }
                vgw.Dir(Qt::Vertical);
            }

        } else {
            if(utils_->doesDockWidgetExist(CHANNEL, upperRight_)){
                dock = utils_->getDockWidget(CHANNEL, upperRight_);
            } else {
                if(utils_->doesDockWidgetExist(SEGMENTATION, upperRight_)){
                    dock = utils_->getDockWidget(SEGMENTATION, upperRight_);
                }
            }
            vgw.Dir(Qt::Vertical);
        }

        return dock;
    }

    QDockWidget* chooseDockToTabify(ViewGroupWidgetInfo& vgw)
    {
        if(VIEW3D == vgw.widgetType){
            return NULL;
        }

        QDockWidget* widgetToTabify = NULL;
        const QString complimentaryObjName = utils_->makeComplimentaryObjectName(vgw);

        if(utils_->doesDockWidgetExist(complimentaryObjName)){
            widgetToTabify = utils_->getDockWidget(complimentaryObjName);
        }

        return widgetToTabify;
    }

public:
    ViewType UpperLeft() const {
        return upperLeft_;
    }

    ViewType UpperRight() const {
        return upperRight_;
    }

    ViewType LowerLeft() const {
        return lowerLeft_;
    }
};

} // namespace gui
} // namespace om

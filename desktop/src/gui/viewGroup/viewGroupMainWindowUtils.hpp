#pragma once

#include "gui/menubar.h"
#include "gui/viewGroup/viewGroupUtils.hpp"

namespace om {
namespace gui {

class ViewGroupMainWindowUtils {
 private:
  MainWindow* const mainWindow_;
  OmViewGroupState* const vgs_;
  ViewGroupUtils* const utils_;

  static const om::common::ViewType upperLeft_ = om::common::XY_VIEW;
  static const om::common::ViewType upperRight_ = om::common::ZY_VIEW;
  static const om::common::ViewType lowerLeft_ = om::common::XZ_VIEW;

 public:
  ViewGroupMainWindowUtils(MainWindow* mainWindow, OmViewGroupState* vgs,
                           ViewGroupUtils* utils)
      : mainWindow_(mainWindow), vgs_(vgs), utils_(utils) {}

  void InsertDockIntoGroup2View(ViewGroupWidgetInfo& vgw,
                                const om::common::ViewType viewType) {
    if (0 == getNumDockWidgets()) {
      QDockWidget* dock = makeDockWidget(vgw);
      mainWindow_->addDockWidget(Qt::BottomDockWidgetArea, dock);
      return;
    }

    QDockWidget* dockToSplit = NULL;

    if (utils_->doesDockWidgetExist(om::common::CHANNEL, viewType)) {
      dockToSplit = utils_->getDockWidget(om::common::CHANNEL, viewType);

    } else if (utils_->doesDockWidgetExist(om::common::SEGMENTATION,
                                           viewType)) {
      dockToSplit = utils_->getDockWidget(om::common::SEGMENTATION, viewType);

    } else if (utils_->doesDockWidgetExist(om::common::AFFINITY, viewType)) {
      dockToSplit = utils_->getDockWidget(om::common::AFFINITY, viewType);

    } else {
      throw om::ArgException("don't know where to put dock");
    }

    InsertBySplitting(vgw, dockToSplit);
  }

  DockWidgetPair InsertDockIntoGroup4View(ViewGroupWidgetInfo& vgw) {
    QDockWidget* dock = NULL;
    QDockWidget* dockToTabify = NULL;

    if (0 == getNumDockWidgets()) {
      dock = makeDockWidget(vgw);
      mainWindow_->addDockWidget(Qt::BottomDockWidgetArea, dock);

    } else {
      dockToTabify = chooseDockToTabify(vgw);

      if (dockToTabify) {
        dock = insertByTabbing(vgw, dockToTabify);

      } else {
        dock = InsertBySplitting(vgw, chooseDockToSplit(vgw));
      }
    }

    DockWidgetPair ret = { dock, dockToTabify };

    return ret;
  }

  QDockWidget* InsertBySplitting(ViewGroupWidgetInfo& vgw,
                                 QDockWidget* biggest) {
    QList<QDockWidget*> tabified = mainWindow_->tabifiedDockWidgets(biggest);

    if (!tabified.empty()) {
      Q_FOREACH(QDockWidget * widget, tabified) {
        mainWindow_->removeDockWidget(widget);
      }
    }

    const Qt::Orientation dir = vgw.Dir();

    QDockWidget* dock = makeDockWidget(vgw);

    //debug(viewGroup, "\t inserting %s by splitting...\n",
    //qPrintable(dock->objectName()));

    mainWindow_->splitDockWidget(biggest, dock, dir);

    if (!tabified.empty()) {
      Q_FOREACH(QDockWidget * dwidget, tabified) {
        dwidget->show();
        mainWindow_->tabifyDockWidget(biggest, dwidget);
      }
    }

    return dock;
  }

 private:
  int getNumDockWidgets() {
    QList<QDockWidget*> widgets = utils_->getAllDockWidgets();
    return widgets.size();
  }

  QDockWidget* insertByTabbing(ViewGroupWidgetInfo& vgw,
                               QDockWidget* widgetToTabify) {
    QDockWidget* dock = makeDockWidget(vgw);
    //debug(viewGroup, "\t inserting %s by tabbing...\n",
    //qPrintable(dock->objectName()));
    mainWindow_->tabifyDockWidget(widgetToTabify, dock);

    return dock;
  }

  QDockWidget* makeDockWidget(ViewGroupWidgetInfo& vgw) {
    QDockWidget* dock = new QDockWidget(vgw.name, mainWindow_);
    vgw.widget->setParent(dock);

    dock->setObjectName(utils_->makeObjectName(vgw));
    dock->setWidget(vgw.widget);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setAttribute(Qt::WA_DeleteOnClose);

    // mainWindow_->mMenuBar->GetWindowMenu()->addAction(dock->toggleViewAction());

    return dock;
  }

  QDockWidget* chooseDockToSplit(ViewGroupWidgetInfo& vgw) {
    QDockWidget* dock = utils_->getBiggestDockWidget();

    if (VIEW2D_CHAN == vgw.widgetType) {
      if (upperLeft_ == vgw.viewType) {

      } else if (upperRight_ == vgw.viewType) {
        if (utils_->doesDockWidgetExist(om::common::CHANNEL, upperLeft_)) {
          dock = utils_->getDockWidget(om::common::CHANNEL, upperLeft_);
        }
      } else {
        if (utils_->doesDockWidgetExist(om::common::CHANNEL, upperLeft_)) {
          dock = utils_->getDockWidget(om::common::CHANNEL, upperLeft_);
        }
        vgw.Dir(Qt::Vertical);
      }

    } else if (VIEW2D_SEG == vgw.widgetType) {
      if (upperLeft_ == vgw.viewType) {

      } else if (upperRight_ == vgw.viewType) {
        if (utils_->doesDockWidgetExist(om::common::SEGMENTATION, upperLeft_)) {
          dock = utils_->getDockWidget(om::common::SEGMENTATION, upperLeft_);
        }
      } else {
        if (utils_->doesDockWidgetExist(om::common::SEGMENTATION, upperLeft_)) {
          dock = utils_->getDockWidget(om::common::SEGMENTATION, upperLeft_);
        }
        vgw.Dir(Qt::Vertical);
      }

    } else {
      if (utils_->doesDockWidgetExist(om::common::CHANNEL, upperRight_)) {
        dock = utils_->getDockWidget(om::common::CHANNEL, upperRight_);
      } else {
        if (utils_->doesDockWidgetExist(om::common::SEGMENTATION,
                                        upperRight_)) {
          dock = utils_->getDockWidget(om::common::SEGMENTATION, upperRight_);
        }
      }
      vgw.Dir(Qt::Vertical);
    }

    return dock;
  }

  QDockWidget* chooseDockToTabify(ViewGroupWidgetInfo& vgw) {
    if (VIEW3D == vgw.widgetType) {
      return NULL;
    }

    QDockWidget* widgetToTabify = NULL;
    const QString complimentaryObjName =
        utils_->makeComplimentaryObjectName(vgw);

    if (utils_->doesDockWidgetExist(complimentaryObjName)) {
      widgetToTabify = utils_->getDockWidget(complimentaryObjName);
    }

    return widgetToTabify;
  }

 public:
  om::common::ViewType UpperLeft() const { return upperLeft_; }

  om::common::ViewType UpperRight() const { return upperRight_; }

  om::common::ViewType LowerLeft() const { return lowerLeft_; }
};

}  // namespace gui
}  // namespace om

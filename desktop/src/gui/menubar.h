#pragma once

#include <QtGui>

#include "gui/recentFileList.h"

class MainWindow;

class MenuBar : public QWidget {
  Q_OBJECT;
  ;
 public:
  MenuBar(MainWindow* mw);

  void UpdateReadOnlyRelatedWidgets();
  void AddRecentFile(const QString& fnp);

  inline QMenu* GetWindowMenu() const { return windowMenu_; }

  inline QAction* GetOpenCacheMonitorAct() { return openCacheMonitorAct_; }

 private:
  MainWindow* const mainWindow_;

  void createActions();
  void createMenus();

  QMenu* fileMenu_;
  QMenu* editMenu_;
  QMenu* projectMenu_;
  QMenu* toolMenu_;
  QMenu* windowMenu_;

  QAction* newAct_;
  QAction* openAct_;
  QAction* saveAct_;
  QAction* closeAct_;
  QAction* quitAct_;

  QAction* editPreferencesAct_;
  QAction* editLocalPreferencesAct_;

  QAction* addChannelAct_;
  QAction* addSegmentationAct_;
  QAction* dumpActionLogAct_;

  QAction* openOmniInspector_;
  QAction* openUndoViewAct_;
  QAction* openCacheMonitorAct_;

  QAction* open3DAct_;

  RecentFileList recentFiles_;
};

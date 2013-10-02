#pragma once

#include "common/common.h"

#include <QMainWindow>
#include <QFrame>
#include <QUndoView>
#include <QLabel>

class CacheMonitorDialog;
class GroupsTable;
class InspectorWidget;
class MainWindowEvents;
class MenuBar;
class OmGlobalKeyPress;
class OmViewGroupState;
class Preferences;
class ToolBarManager;
class ViewGroup;

namespace om {
class Exception;
namespace event {
class SegmentEvent;
}
}

class MainWindow : public QMainWindow {
  Q_OBJECT
public : MainWindow();
  ~MainWindow();

  void openProject(QString fileNameAndPath);
  void openProject(QString fileName, QString pathName);

  void cleanViewsOnVolumeChange(om::common::ObjectType objectType,
                                om::common::ID objectId);
  void updateStatusBar(QString msg);

  inline OmViewGroupState* GetViewGroupState() { return vgs_.get(); }

  void addToolbarTop(QToolBar* b);
  void addToolbarRight(QToolBar* b);

  friend class ViewGroupMainWindowUtils;

 protected:
  void closeEvent(QCloseEvent* event);

  void SegmentModificationEvent(om::event::SegmentEvent* event);

 public
Q_SLOTS:
  void spawnErrorDialog(om::Exception& e);
  void saveProject();

 private
Q_SLOTS:
  void newProject();
  void openProject();
  void openRecentFile();
  void closeProject();

  void openInspector();
  void openUndoView();
  void openCacheMonitor();
  void openGroupsTable();

  void open3dView();

  void showEditPreferencesDialog();
  void showEditLocalPreferencesDialog();
  void addChannelToVolume();
  void addSegmentationToVolume();
  void dumpActionLog();

 private:
  bool editsMade;

  InspectorWidget* inspector_;
  std::unique_ptr<QDockWidget> inspectorDock_;

  QUndoView* undoView_;
  std::unique_ptr<QDockWidget> undoViewDock_;

  GroupsTable* groupsTable_;
  std::unique_ptr<QDockWidget> groupsTableDock_;

  CacheMonitorDialog* cacheMonitorDialog_;

  std::unique_ptr<Preferences> preferences_;

  QLabel* statusBarLabel;

  ToolBarManager* toolBarManager_;
  MenuBar* mMenuBar;

  std::unique_ptr<OmViewGroupState> vgs_;

  QAction* panAct;
  QAction* zoomAct;

  bool closeProjectIfOpen();
  int checkForSave();
  void createStatusBar();
  void resetViewGroup();

  void updateGuiFromProjectCreateOrOpen(QString fileName);
  void updateReadOnlyRelatedWidgets();
  void windowTitleClear();
  void windowTitleSet(QString title);

  QToolBar* fakeToolbarForMac_;

  std::unique_ptr<OmGlobalKeyPress> globalKeys_;
  std::unique_ptr<MainWindowEvents> events_;
};

#pragma once

#include "common/common.h"

#include <QMainWindow>
#include <QFrame>
#include <QUndoView>
#include <QLabel>

class CacheMonitorDialog;
class InspectorWidget;
class MainWindowEvents;
class MenuBar;
class OmGlobalKeyPress;
class OmSegmentEvent;
class OmViewGroupState;
class Preferences;
class ToolBarManager;
class LoginToolBar;
class ViewGroup;

class MainWindow : public QMainWindow {
  Q_OBJECT;

 public:
  MainWindow();
  ~MainWindow();

  bool openProject(QString fileNameAndPath);

  void cleanViewsOnVolumeChange(om::common::ObjectType objectType,
                                om::common::ID objectId);
  void updateStatusBar(QString msg);

  inline OmViewGroupState* GetViewGroupState() { return vgs_.get(); }

  void addToolbarTop(QToolBar* b);
  void addToolbarRight(QToolBar* b);

  friend class ViewGroupMainWindowUtils;

 protected:
  void closeEvent(QCloseEvent* event);

  void SegmentModificationEvent(OmSegmentEvent* event);

 public
Q_SLOTS:
  void spawnErrorDialog(om::Exception& e);
  void saveProject();

 private
Q_SLOTS:
  void newProject();
  bool openProject();
  void openRecentFile();
  void closeProject();

  void openInspector();
  void openUndoView();
  void openCacheMonitor();

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

  CacheMonitorDialog* cacheMonitorDialog_;

  std::unique_ptr<Preferences> preferences_;

  QLabel* statusBarLabel;

  MenuBar* mMenuBar;
  LoginToolBar* loginToolBar_;
  ToolBarManager* toolBarManager_;

  std::unique_ptr<OmViewGroupState> vgs_;

  QAction* panAct;
  QAction* zoomAct;

  bool loadProject(std::string fileNameAndPath);
  bool closeProjectIfOpen(bool);
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

#pragma once

#include "common/omCommon.h"

#include <QMainWindow>
#include <QFrame>
#include <QUndoView>
#include <QLabel>

class CacheMonitorDialog;
class GroupsTable;
class InspectorWidget;
class MainWindowEvents;
class MenuBar;
class OmException;
class OmGlobalKeyPress;
class OmSegmentEvent;
class OmViewGroupState;
class Preferences;
class ToolBarManager;
class ViewGroup;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    void openProject( QString fileNameAndPath );
    void openProject( QString fileName, QString pathName );

    void cleanViewsOnVolumeChange(ObjectType objectType, OmID objectId);
    void updateStatusBar( QString msg );

    inline OmViewGroupState* GetViewGroupState(){
        return vgs_.get();
    }

    void addToolbarTop(QToolBar* b);
    void addToolbarRight(QToolBar* b);

    friend class ViewGroupMainWindowUtils;

protected:
    void closeEvent(QCloseEvent* event);

    void SegmentModificationEvent(OmSegmentEvent* event);

public Q_SLOTS:
    void spawnErrorDialog(OmException &e);
    void saveProject();

private Q_SLOTS:
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
    boost::scoped_ptr<QDockWidget> inspectorDock_;

    QUndoView* undoView_;
    boost::scoped_ptr<QDockWidget> undoViewDock_;

    GroupsTable* groupsTable_;
    boost::scoped_ptr<QDockWidget> groupsTableDock_;

    CacheMonitorDialog* cacheMonitorDialog_;

    boost::scoped_ptr<Preferences> preferences_;

    QLabel* statusBarLabel;

    ToolBarManager* toolBarManager_;
    MenuBar* mMenuBar;

    boost::scoped_ptr<OmViewGroupState> vgs_;

    QAction* panAct;
    QAction* zoomAct;

    bool closeProjectIfOpen();
    int checkForSave();
    void createStatusBar();
    void resetViewGroup();

    void updateGuiFromProjectCreateOrOpen( QString fileName );
    void updateReadOnlyRelatedWidgets();
    void windowTitleClear();
    void windowTitleSet(QString title);

    QToolBar* fakeToolbarForMac_;

    boost::scoped_ptr<OmGlobalKeyPress> globalKeys_;
    boost::scoped_ptr<MainWindowEvents> events_;
};


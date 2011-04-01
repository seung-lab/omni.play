#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "common/omCommon.h"

#include <QMainWindow>
#include <QFrame>
#include <QUndoView>
#include <QLabel>

class GroupsTable;
class CacheMonitorDialog;
class MenuBar;
class InspectorWidget;
class OmException;
class OmSegmentEvent;
class OmViewGroupState;
class Preferences;
class ToolBarManager;
class ViewGroup;
class OmGlobalKeyPress;

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    void openProject( QString fileNameAndPath );
    void openProject( QString fileName, QString pathName );

    void cleanViewsOnVolumeChange(ObjectType objectType, OmID objectId);
    void updateStatusBar( QString msg );

    inline OmViewGroupState* getViewGroupState(){
        return vgs_.get();
    }

    void addToolbarTop(QToolBar* b);
    void addToolbarRight(QToolBar* b);

    friend class ViewGroup;

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

    QFrame* loadingDock_;

    bool closeProjectIfOpen();
    int checkForSave();
    void createStatusBar();
    void moveEvent(QMoveEvent* event);
    void resetViewGroup();
    void resizeEvent(QResizeEvent* event);
    void updateGuiFromProjectLoadOrOpen( QString fileName );
    void updateReadOnlyRelatedWidgets();
    void windowTitleClear();
    void windowTitleSet(QString title);

    QToolBar* fakeToolbarForMac_;

    boost::scoped_ptr<OmGlobalKeyPress> globalKeys_;
};

#endif

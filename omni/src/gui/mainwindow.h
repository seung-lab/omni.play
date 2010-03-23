#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMultiMap>
#include <QProgressDialog>
#include <QThread>
#include <qtconcurrentrun.h>
#include <QList>
#include <QMultiMap>
#include <QUndoStack>
#include <QUndoView>
#include <QComboBox>
#include <QErrorMessage>

#include "gui/viewGroup.h"
#include "common/omStd.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "system/omGenericManager.h"
#include "system/omStateManager.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omProgressEvent.h"
#include "system/events/omAlertEvent.h"
#include "system/events/omSystemModeEvent.h"

#include "gui/recentFileList.h"
#include "gui/preferences/preferences.h"

class MyInspectorWidget;
class ViewGroup;

class MainWindow 
: public QMainWindow, 	
  public OmProgressEventListener,
  public OmAlertEventListener,
  public OmSystemModeEventListener
{
 Q_OBJECT
 
  public:
	MainWindow();
	
	void channel_build(OmChannel *current_channel);
	void seg_build(OmSegmentation *current_seg);
	void mesh_build(OmSegmentation *current_seg, QFuture<void> &last_future);		
		
	void openProject( QString fileNameAndPath );
	void openProject( QString fileName, QString pathName );

	friend class ViewGroup;

 protected: 
	void closeEvent(QCloseEvent *event); 
		
	void SegmentObjectModificationEvent(OmSegmentEvent *event);
	void SystemModeChangeEvent(OmSystemModeEvent * event);		
	void AlertNotifyEvent(OmAlertEvent *event);
		
	void ProgressShow(OmProgressEvent *event);
	void ProgressHide(OmProgressEvent *event);
		
	void ProgressRangeEvent(OmProgressEvent *event);
	void ProgressValueEvent(OmProgressEvent *event);
	void ProgressIncrementEvent(OmProgressEvent *event);	
		
 private slots:
	void newProject();
	void openProject();
	void openRecentFile();
	void closeProject();
	void saveProject();
		
	void openInspector();
	void openUndoView();
		
	void open3dView();		
	void openChannelView(OmId chan_id, ViewType vtype);
	void openSegmentationView(OmId primary_id, ViewType vtype);
		
	void showEditPreferencesDialog();
	void showEditLocalPreferencesDialog();
	void addChannelToVolume();
	void addSegmentationToVolume();
		
	void updateStatusBar(); 	
	void updateKeyShortcuts();		
		
	void spawnErrorDialog(OmException &e);

	void ChangeModeModify(bool checked);
	void toolbarSelect(bool checked);
	void toolbarCrosshair(bool checked);
	void toolbarPan(bool checked);
	void toolbarZoom(bool checked);
	void toolbarBrush(bool checked);
	void toolbarEraser(bool checked);
	void toolbarFill(bool checked);
	void toolbarVoxelize(bool checked);

	void open2Dand3dViews();	
			
 private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
		
	bool checkForSave();
		
	QFrame *loadingDock;
		
	QMenu *fileMenu;
	QMenu *editMenu;
	QMenu *projectMenu;
	QMenu *toolMenu;
	QMenu *windowMenu;
	
	QToolBar *fileToolBar;
	QToolBar *systemToolBar;
	QToolBar *toolToolBar;
	QToolBar *navigateToolBar;
	QToolBar *editToolBar;
	QToolBar *viewToolBar;
		
	QAction *newAct;
	QAction *openAct;
	QAction *saveAct;
	QAction *closeAct;
	QAction *quitAct;
		
	QAction *editPreferencesAct;
	QAction *editLocalPreferencesAct;

	QAction *addChannelAct;
	QAction *addSegmentationAct;
		
	QAction *openOmniInspector;
	QAction *openUndoViewAct;
		
	QAction *open3DAct;
		
	// private variables
	QErrorMessage *exceptionMessage;
		
	QProgressDialog prog_dialog;
	bool editsMade;
		
	MyInspectorWidget *omniInspector;
	QUndoView *undoView;
		
	bool isProjectOpen;
	bool closeProjectIfOpen();
		
	RecentFileList recentFiles;

	// new toolbar
	void createToolbar();
	void createToolbarActions();
	void addToolbars();
	void setupSegmentationBoxes();
	QAction *modifyAct;
	QAction *toolbarSelectAct;
	QAction *toolbarCrosshairAct;
	QAction *toolbarStickyCrosshairAct;
	QAction *toolbarPanAct;
	QAction *toolbarZoomAct;
	QAction *toolbarBrushAct;
	QAction *toolbarEraserAct;
	QAction *toolbarFillAct;
	QAction *toolbarVoxelizeAct;

	QAction *toolbarView2D3DopenAct;

	QAction *panAct;
	QAction *zoomAct;
	void resetTool( QAction* tool, const bool enabled );
	void resetTools( const OmSystemMode sys_mode );
	void resetViewTools();
	void resetModifyTools( const bool enabled );
	void setupToolbarInitially();
	void toolbarToolChange(const bool checked, QAction * tool, const OmToolMode mode );

	Preferences* preferences;
	void windowTitleSet(QString title);
	void windowTitleClear();
	void updateReadOnlyRelatedWidgets();

	ViewGroup * mViewGroup;
	void resetViewGroup();

	void updateGuiFromPorjectLoadOrOpen( QString fileName );

};

#endif

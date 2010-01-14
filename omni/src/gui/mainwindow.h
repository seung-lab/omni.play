#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMultiMap>
#include <QProgressDialog>
#include <QThread>
#include <qtconcurrentrun.h>

#include "common/omStd.h"
#include "system/omGenericManager.h"
#include "system/omStateManager.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omProgressEvent.h"
#include "system/events/omAlertEvent.h"
#include "system/events/omSystemModeEvent.h"

#include "gui/recentFileList.h"

class QAction;
class QMenu;
class QFrame;
class QUndoStack;
class QUndoView;

class QErrorMessage;

// the 2D viewing widget
class OmView2d;
class DOmView2d;

// the 3D viewing widget
class OmView3d;

// the Omni Inspector
class MyInspectorWidget;

class TreeModel;

class QPushButton;
class QComboBox;

class OmChannel;
class OmSegmentation;

class MainWindow 
: public QMainWindow, 	
  public OmSegmentEventListener,
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
		
		//open project at path
		void openProject( QString fileNameAndPath );
		void openProject( QString fileName, QString pathName );
		
	protected: 
		void closeEvent(QCloseEvent *event); 
		
		void SegmentObjectModificationEvent(OmSegmentEvent *event);
		void SegmentEditSelectionChangeEvent(OmSegmentEvent *event);
		
		void SystemModeChangeEvent(OmSystemModeEvent *event);
		
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
		bool saveProject();
		
		void openInspector();
		void openUndoView();
		
		void open3dView();
		
		void openChannelView(OmId chan_id, OmId second_chan_id, OmId third_id, ViewType vtype);
		void openSegmentationView(OmId primary_id, OmId secondary_id, ViewType vtype);
		
		void addChannelToVolume();
		void addSegmentationToVolume();
		
		void buildAll();
		
		void updateStatusBar(); 
		
		void updateKeyShortcuts();
		
		void changeSelection(int segmentIndex);
		
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
		void updateComboBoxes( const OmId segmentationID = 0, 
						   const OmId segmentJustSelectedID = 0 );
				
	private:
		void createActions();
		void createMenus();
		void createToolBars();
		void createStatusBar();
		void createDockWindows();
		
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
		
		QAction *newAct;
		QAction *openAct;
		QAction *saveAct;
		QAction *closeAct;
		QAction *quitAct;
		
		QComboBox *selectSegmentationBox;
		QPushButton *editColorButton;
		
		QAction *addChannelAct;
		QAction *addSegmentationAct;
		QAction *buildAllAct;
		
		QAction *openOmniInspector;
		QAction *openUndoViewAct;
		
		QAction *open3DAct;
		
		// private variables
		QErrorMessage *exceptionMessage;
		
		QProgressDialog prog_dialog;
		bool editsMade;
		
		bool iSentIt;
		
		OmView3d *qtView3d;
		OmView2d *qtView2d;
		DOmView2d *myView2d;
		
		MyInspectorWidget *omniInspector;
		QUndoView *undoView;
		
		bool isProjectOpen;
		bool closeProjectIfOpen();
		
		QColor selected_color;

		RecentFileList recentFiles;

		// new toolbar
		void createToolbar();
		void createToolbarActions();
		void addToolbars();
		void setupSegmentationBoxes();
		QAction *modifyAct;
		QAction *toolbarSelectAct;
		QAction *toolbarCrosshairAct;
		QAction *toolbarPanAct;
		QAction *toolbarZoomAct;
		QAction *toolbarBrushAct;
		QAction *toolbarEraserAct;
		QAction *toolbarFillAct;
		QAction *toolbarVoxelizeAct;

		QAction *panAct;
		QAction *zoomAct;
		void resetTool( QAction* tool, const bool enabled );
		void resetTools( const OmSystemMode sys_mode );
		void resetViewTools();
		void resetModifyTools( const bool enabled );
		void setupToolbarInitially();
		void toolbarToolChange(const bool checked, QAction * tool, const OmToolMode mode);
	};

#endif

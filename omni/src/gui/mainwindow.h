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
#include "gui/toolbars/toolbarManager.h"
#include "gui/menubar.h"
#include "common/omStd.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "system/omGenericManager.h"
#include "system/omStateManager.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omProgressEvent.h"
#include "system/events/omSystemModeEvent.h"
#include "system/viewGroup/omViewGroupState.h"

#include "gui/preferences/preferences.h"

class MyInspectorWidget;
class ViewGroup;
class CacheMonitorDialog;

class MainWindow 
: public QMainWindow, 	
  //  public OmProgressEventListener,
  public OmSystemModeEventListener
{
 Q_OBJECT
 
  public:
	MainWindow();

	void openProject( QString fileNameAndPath );
	void openProject( QString fileName, QString pathName );

	void cleanViewsOnVolumeChange(ObjectType objectType, OmId objectId);
	void updateStatusBar( QString msg );

	bool isProjectOpen();
	
	friend class ViewGroup;

 protected: 
	void closeEvent(QCloseEvent *event); 
		
	void SegmentObjectModificationEvent(OmSegmentEvent *event);
	void SystemModeChangeEvent();		

 public slots:
	void spawnErrorDialog(OmException &e);
	void saveProject();
	void open2Dand3dViews();

 private slots:
	void newProject();
	void openProject();
	void openRecentFile();
	void closeProject();
		
	void openInspector();
	void openUndoView();
	void openCacheMonitor();
		
	void open3dView();
	void openChannelView(OmId chan_id, ViewType vtype);
	void openSegmentationView(OmId primary_id, ViewType vtype);
		
	void showEditPreferencesDialog();
	void showEditLocalPreferencesDialog();
	void addChannelToVolume();
	void addSegmentationToVolume();
				
 private:
	bool checkForSave();
		
	QFrame *loadingDock;
				
	QErrorMessage *exceptionMessage;
		
	QProgressDialog prog_dialog;
	bool editsMade;
		
	MyInspectorWidget *omniInspector;
	QUndoView *undoView;
	CacheMonitorDialog *mCacheMonitorDialog; 
		
	void setProjectOpen(bool open);
	bool mIsProjectOpen;
	bool closeProjectIfOpen();
		
	QAction *panAct;
	QAction *zoomAct;

	Preferences* preferences;
	void windowTitleSet(QString title);
	void windowTitleClear();
	void updateReadOnlyRelatedWidgets();

	ViewGroup * mViewGroup;
	void resetViewGroup();

	void updateGuiFromProjectLoadOrOpen( QString fileName );

	QLabel * statusBarLabel;

	void createStatusBar();

	ToolBarManager * mToolBars;
	MenuBar * mMenuBar;

	OmViewGroupState * mViewGroupState;
};

#endif

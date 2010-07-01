#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMultiMap>
#include <QProgressDialog>
#include <QThread>
#include <QList>
#include <QMultiMap>
#include <QUndoStack>
#include <QUndoView>
#include <QComboBox>
#include <QErrorMessage>

#include "common/omCommon.h"
#include "system/events/omProgressEvent.h"

class CacheMonitorDialog;
class MenuBar;
class MyInspectorWidget;
class OmException;
class OmSegmentEvent;
class OmViewGroupState;
class Preferences;
class ToolBarManager;
class ViewGroup;

class MainWindow 
: public QMainWindow
  //  public OmProgressEventListener,
{
 Q_OBJECT
 
  public:
	MainWindow();

	void openProject( QString fileNameAndPath );
	void openProject( QString fileName, QString pathName );

	void cleanViewsOnVolumeChange(ObjectType objectType, OmId objectId);
	void updateStatusBar( QString msg );

	bool isProjectOpen();

	OmViewGroupState * getViewGroupState(){ return mViewGroupState; }

	void addToolbarWidget(QWidget * b);
	void addToolbarSeperator();

	void addToolbarRight(QToolBar * b);
	
	friend class ViewGroup;

 protected: 
	void closeEvent(QCloseEvent *event); 
		
	void SegmentObjectModificationEvent(OmSegmentEvent *event);

 public slots:
	void spawnErrorDialog(OmException &e);
	void saveProject();

 private slots:
	void newProject();
	void openProject();
	void openRecentFile();
	void closeProject();
		
	void openInspector();
	void openUndoView();
	void openCacheMonitor();
	void openGroupsTable();

		
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

	void resetViewGroup();

	void updateGuiFromProjectLoadOrOpen( QString fileName );

	QLabel * statusBarLabel;

	void createStatusBar();

	ToolBarManager * mToolBars;
	MenuBar * mMenuBar;

	OmViewGroupState * mViewGroupState;
	QToolBar *mToolToolBar;

};

#endif

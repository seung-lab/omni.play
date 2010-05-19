#include "myInspectorWidget.h"
#include "mainwindow.h"
#include "recentFileList.h"
#include "gui/cacheMonitorDialog.h"
#include "utility/dataWrappers.h"
#include "project/omProject.h"
#include "system/omProjectData.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include "system/omEventManager.h"
#include "system/events/omSystemModeEvent.h"
#include "system/events/omToolModeEvent.h"
#include "segment/omSegmentEditor.h"

#include "common/omException.h"

Q_DECLARE_METATYPE(SegmentDataWrapper);

MainWindow::MainWindow()
 : prog_dialog(this)
{
	exceptionMessage = new QErrorMessage(this);
	OmStateManager::SetMainWindow(this);

	setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
	setAnimated(false);

	setFocusPolicy(Qt::ClickFocus);

	loadingDock = new QFrame;
	loadingDock->setFrameStyle(QFrame::Box | QFrame::Raised);

	QGridLayout *dockLayout = new QGridLayout;

	loadingDock->setLayout(dockLayout);

	mMenuBar = new MenuBar( this );
	mToolBars = NULL;
	mIsProjectOpen = false;

	createStatusBar();

	windowTitleClear();
	resize(1000, 800);

	preferences = NULL;
	setProjectOpen( false );
	omniInspector = NULL;
	undoView = NULL;
	mViewGroup = NULL;
	mCacheMonitorDialog = NULL;

	mViewGroupState = NULL;

	updateReadOnlyRelatedWidgets();
}

// Creates a new project
// Prompts the user for a directory in which to create the new project
// Internally, this sets the current working directory where the Volume will be serialized
void MainWindow::newProject()
{
	try {

		if (!closeProjectIfOpen()) {
			return;
		}

		QString fileName = QFileDialog::getSaveFileName(this, tr("New Project"));

		if (fileName == NULL) {
			return;
		}

		QString fileNameAndPath = OmProject::New( fileName );

		if( NULL != mViewGroupState ){
			delete mViewGroupState;
		}
		mViewGroupState = new OmViewGroupState();

		mViewGroupState->SetViewSliceDepth(XY_VIEW, 0.0);
		mViewGroupState->SetViewSliceDepth(XZ_VIEW, 0.0);
		mViewGroupState->SetViewSliceDepth(YZ_VIEW, 0.0);
		
		updateGuiFromPorjectLoadOrOpen( fileNameAndPath );

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::showEditPreferencesDialog()
{
	if (!isProjectOpen() ) {
		return;
	}
	
	if (preferences) {
		preferences->close();
		delete(preferences);
		preferences = NULL;
	} 

	preferences = new Preferences(this);
	preferences->showProjectPreferences();
	preferences->show();
	preferences->raise();
	preferences->activateWindow();
}

void MainWindow::showEditLocalPreferencesDialog()
{
	if (preferences) {
		preferences->close();
		delete(preferences);
		preferences = NULL;
	} 

	preferences = new Preferences(this);
	preferences->showLocalPreferences();
	preferences->show();
	preferences->raise();
	preferences->activateWindow();
}

void MainWindow::addChannelToVolume()
{
	try {
		if (!isProjectOpen() ) {
			return;
		}

		if (omniInspector) {
			omniInspector->addChannelToVolume();
		}
		
	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::addSegmentationToVolume()
{
	try {
		if (!isProjectOpen() ) {
			return;
		}

		if (omniInspector) {
			omniInspector->addSegmentationToVolume();
		}

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

/* returns false if New/Open action is cancelled by user */
bool MainWindow::closeProjectIfOpen()
{

	if (!isProjectOpen() ) {
		return true;
	}

	if (!checkForSave()) {
		return false;
	}

	setProjectOpen( false );

	// get rid of QDockWidget that may contain Inspector, History, View, etc widgets        
	foreach( QDockWidget * dw, this->findChildren<QDockWidget *>() ){
		delete(dw);
	}

	if (preferences) {
		preferences->close();
		delete(preferences);
		preferences = NULL;
	}

	OmProject::Close();
	windowTitleClear();

	updateReadOnlyRelatedWidgets();

	return true;
}

void MainWindow::openRecentFile()
{
	QAction *action = qobject_cast < QAction * >(sender());

	if (!action) {
		return;
	}

	if (!closeProjectIfOpen()) {
		return;
	}

	openProject(action->data().toString());
}

void MainWindow::openProject()
{
	try {
		if (!closeProjectIfOpen()) {
			return;
		}
		// Opens an existing project
		// Prompts the user for their project file
		// Internally, this is the file that has Volume serialized

		QString fileNameAndPath = QFileDialog::getOpenFileName(this,
								       tr("Open Project"), "",
								       tr("Project File (*.omni)"));

		if (fileNameAndPath == NULL)
			return;

		//open project at fpath
		openProject(fileNameAndPath);
	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::openProject(QString fileNameAndPath)
{
	try {
		try {
			OmProject::Load( fileNameAndPath );
		} catch(...) {
			throw OmIoException("error during load of OmProject object");
		}

#if 0
		// FIXME open volume at middle.... (purcaro)
		SpaceCoord depth = OmVolume::NormToSpaceCoord( NormCoord(0.5, 0.5, 0.5));
#endif
		if( NULL != mViewGroupState ){
			delete mViewGroupState;
		}
		mViewGroupState = new OmViewGroupState();

		mViewGroupState->SetViewSliceDepth(XY_VIEW, 0.0);
		mViewGroupState->SetViewSliceDepth(XZ_VIEW, 0.0);
		mViewGroupState->SetViewSliceDepth(YZ_VIEW, 0.0);		

		updateGuiFromPorjectLoadOrOpen( fileNameAndPath );

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::closeProject()
{
	try {
		closeProjectIfOpen();

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::saveProject()
{
	// Saves the current project
	// Does not prompt the user
	// Serializes Volume to the current working directory
	OmProject::Save();
}

void MainWindow::openInspector()
{
	try {

		if (!isProjectOpen()) {
			return;
		}

		resetViewGroup();

		omniInspector = new MyInspectorWidget( this );
		omniInspector->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Expanding );

		QDockWidget *dock = new QDockWidget(tr("Inspector"), this);
		dock->setAllowedAreas(Qt::AllDockWidgetAreas);

		omniInspector->setParent(dock);
		dock->setWidget(omniInspector);

		addDockWidget(Qt::TopDockWidgetArea, dock);
		mMenuBar->getWindowMenu()->addAction(dock->toggleViewAction());

		connect(omniInspector, SIGNAL(triggerChannelView(OmId, ViewType)),
			this, SLOT(openChannelView(OmId, ViewType)));
		connect(omniInspector, SIGNAL(triggerSegmentationView(OmId, ViewType)),
			this, SLOT(openSegmentationView(OmId, ViewType)));

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::openUndoView()
{
	try {
		undoView = new QUndoView();

		QDockWidget *dock = new QDockWidget(tr("History"), this);
		dock->setAllowedAreas(Qt::AllDockWidgetAreas);

		undoView->setParent(dock);
		dock->setWidget(undoView);

		addDockWidget(Qt::TopDockWidgetArea, dock);
		mMenuBar->getWindowMenu()->addAction(dock->toggleViewAction());

		undoView->setStack(OmStateManager::GetUndoStack());

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::open3dView()
{
	try {
		if (!isProjectOpen()) {
			return;
		}

		mViewGroup->addView3D();

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::openChannelView(OmId chan_id, ViewType vtype)
{
	try {
		mViewGroup->addView2Dchannel( chan_id, vtype);

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::openSegmentationView(OmId segmentation_id, ViewType vtype)
{
	try {
		mViewGroup->addView2Dsegmentation( segmentation_id, vtype);

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	try {
		// QMainWindow::saveState() and restoreState()

		if (isProjectOpen()) {
			if (!checkForSave()) {
				event->ignore();
			}
		}

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

bool MainWindow::checkForSave()
{
	QMessageBox msgBox;
	msgBox.setText("Do you want to save your current project?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Save);
	
	const int ret = msgBox.exec();
	switch (ret) {
	case QMessageBox::Save:
		OmProject::Save();
		break;
	case QMessageBox::Discard:
		// Don't Save was clicked
		break;
	case QMessageBox::Cancel:
		return false;
		break;
	default:
		break;
	}
	
	return true;
}

void MainWindow::spawnErrorDialog(OmException & e)
{
	//assert (0);

	QString errorMessage = e.GetType() + ": " + e.GetName() + ". " + e.GetMsg();
	exceptionMessage->showMessage(errorMessage);
	printf("something bad happened in %s:, \n\t%s\n", __FUNCTION__, qPrintable(errorMessage) );
}

void MainWindow::updateReadOnlyRelatedWidgets()
{
	mMenuBar->updateReadOnlyRelatedWidgets();

	if( mToolBars != NULL ){
		mToolBars->updateReadOnlyRelatedWidgets();
	}
}

void MainWindow::windowTitleSet(QString title)
{
	QString str = "Omni - " + title;
	
	if( OmProjectData::IsReadOnly() ){
		str = "[READ-ONLY] " + str;
	}
	
	setWindowTitle( str );
}

void MainWindow::windowTitleClear()
{
	setWindowTitle(tr("Omni"));
}

void MainWindow::resetViewGroup()
{
	if( mViewGroup != NULL ){
		delete(mViewGroup);
	}

	mViewGroup = new ViewGroup( this, mViewGroupState );
}

void MainWindow::updateGuiFromPorjectLoadOrOpen( QString fileName )
{
	if( NULL == mToolBars ){
		mToolBars = new ToolBarManager(this);
	}

	mMenuBar->addRecentFile(fileName);
	setProjectOpen( true );

	mViewGroupState->SetViewSliceMin(XY_VIEW, Vector2 < float >(0.0, 0.0));
	mViewGroupState->SetViewSliceMin(XZ_VIEW, Vector2 < float >(0.0, 0.0));
	mViewGroupState->SetViewSliceMin(YZ_VIEW, Vector2 < float >(0.0, 0.0));

	mViewGroupState->SetViewSliceMax(XY_VIEW, Vector2 < float >(0.0, 0.0));
	mViewGroupState->SetViewSliceMax(XZ_VIEW, Vector2 < float >(0.0, 0.0));
	mViewGroupState->SetViewSliceMax(YZ_VIEW, Vector2 < float >(0.0, 0.0));

	mViewGroupState->SetZoomLevel(Vector2 < int >(0, 10));
	mViewGroupState->SetPanDistance(XY_VIEW, Vector2 < int >(0, 0));
	mViewGroupState->SetPanDistance(XZ_VIEW, Vector2 < int >(0, 0));
	mViewGroupState->SetPanDistance(YZ_VIEW, Vector2 < int >(0, 0));

	mToolBars->setupToolbarInitially();
	mToolBars->updateGuiFromPorjectLoadOrOpen();

	windowTitleSet( fileName );
	openInspector();
	updateReadOnlyRelatedWidgets();
}

void MainWindow::open2Dand3dViews()
{
	OmId channelID = 1;
	OmId segmentationID = 1;

	mViewGroup->addAllViews( channelID, segmentationID );
}

void MainWindow::openCacheMonitor()
{
	mCacheMonitorDialog = new CacheMonitorDialog( this );
	mCacheMonitorDialog->setAttribute(Qt::WA_DeleteOnClose, true);

	mCacheMonitorDialog->show();
	mMenuBar->getOpenCacheMonitorAct()->setChecked(true);
}

void MainWindow::cleanViewsOnVolumeChange(ObjectType objectType, OmId objectId )
{
	QString unwantedView3DTitle = "3D";
	foreach( QDockWidget * dw, this->findChildren<QDockWidget *>() ){

		if(dw->windowTitle().startsWith(unwantedView3DTitle) ){
			delete(dw);
		}
	}

	QString unwantedView2DTitle;
	switch (objectType){
	case CHANNEL:
		unwantedView2DTitle = "channel" + QString::number(objectId);
		break;
	case SEGMENTATION:
		unwantedView2DTitle = "segmentation" + QString::number(objectId);
		break;
	default:
		return;
	}

	foreach( QDockWidget * dw, this->findChildren<QDockWidget *>() ){

		if (dw->windowTitle().startsWith(unwantedView2DTitle) ){		   
			delete(dw);
		}
	}
}

void MainWindow::createStatusBar()
{
	statusBarLabel = new QLabel(this);
	statusBarLabel->setText("");
	statusBar()->addWidget( statusBarLabel );
}

void MainWindow::updateStatusBar( QString msg )
{
	statusBarLabel->setText(msg);
}

void MainWindow::SystemModeChangeEvent()
{
	mToolBars->SystemModeChangeEvent();
}

bool MainWindow::isProjectOpen()
{
	return mIsProjectOpen;
}

void MainWindow::setProjectOpen(bool open)
{
	mIsProjectOpen = open;
}

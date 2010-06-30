#include "common/omException.h"
#include "gui/cacheMonitorDialog.h"
#include "gui/menubar.h"
#include "gui/preferences/preferences.h"
#include "gui/toolbars/toolbarManager.h"
#include "mainwindow.h"
#include "myInspectorWidget.h"
#include "project/omProject.h"
#include "recentFileList.h"
#include "segment/actions/omSegmentEditor.h"
#include "system/omEventManager.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"

MainWindow::MainWindow()
 : prog_dialog(this)
{
	exceptionMessage = new QErrorMessage(this);

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
	resize(1000, 1000);

	preferences = NULL;
	setProjectOpen( false );
	omniInspector = NULL;
	undoView = NULL;
	mCacheMonitorDialog = NULL;

	mViewGroupState = NULL;
	mToolToolBar = addToolBar("Tools");
	updateReadOnlyRelatedWidgets();

	OmStateManager::setMainWindow(this);
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

		updateGuiFromProjectLoadOrOpen( fileNameAndPath );

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

	delete mViewGroupState;
	mViewGroupState = NULL;

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
		OmProject::Load( fileNameAndPath );
		updateGuiFromProjectLoadOrOpen( fileNameAndPath );

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

		omniInspector = new MyInspectorWidget( this, mViewGroupState );
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

		mViewGroupState->addView3D();

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::openChannelView(OmId chan_id, ViewType vtype)
{
	try {
		mViewGroupState->addView2Dchannel( chan_id, vtype);

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::openSegmentationView(OmId segmentation_id, ViewType vtype)
{
	try {
		mViewGroupState->addView2Dsegmentation( segmentation_id, vtype);

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
	exceptionMessage->showMessage(errorMessage, QDateTime::currentDateTime().toString() ); // force user to always see dialog
	printf("Exception thrown: %s\n", qPrintable(errorMessage) );
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

void MainWindow::updateGuiFromProjectLoadOrOpen( QString fileName )
{
	delete mViewGroupState;
	mViewGroupState = new OmViewGroupState(this);
	
	if( NULL == mToolBars ){
		mToolBars = new ToolBarManager(this);
	}

	mMenuBar->addRecentFile(fileName);
	setProjectOpen( true );

	mToolBars->setupToolbarInitially();
	mToolBars->updateGuiFromProjectLoadOrOpen(mViewGroupState);

	windowTitleSet( fileName );
	openInspector();
	updateReadOnlyRelatedWidgets();
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

void MainWindow::addToolbarWidget(QWidget * b)
{
	mToolToolBar->addWidget(b);
}

void MainWindow::addToolbarSeperator()
{
	mToolToolBar->addSeparator();
}

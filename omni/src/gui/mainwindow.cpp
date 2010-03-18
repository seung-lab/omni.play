#include "myInspectorWidget.h"
#include "mainwindow.h"
#include "recentFileList.h"

#include "view2d/omTile.h"
#include "view2d/omTextureID.h"
#include "view2d/omView2d.h"
#include "view3d/omView3d.h"
#include "project/omProject.h"
#include "volume/omChannel.h"
#include "volume/omVolume.h"
#include "utility/dataWrappers.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omEventManager.h"
#include "system/events/omSystemModeEvent.h"
#include "system/events/omToolModeEvent.h"
#include "system/omSystemTypes.h"
#include "segment/omSegmentEditor.h"

#include "common/omException.h"

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs = boost::filesystem;

#include <boost/tuple/tuple_comparison.hpp>
#include "common/omDebug.h"
using boost::tuple;

Q_DECLARE_METATYPE(SegmentDataWrapper);

MainWindow::MainWindow()
 : prog_dialog(this)
{
	exceptionMessage = new QErrorMessage(this);

	try {

		setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
		setAnimated(false);

		setFocusPolicy(Qt::ClickFocus);

		loadingDock = new QFrame;
		loadingDock->setFrameStyle(QFrame::Box | QFrame::Raised);

		QGridLayout *dockLayout = new QGridLayout;

		loadingDock->setLayout(dockLayout);

		createActions();
		createMenus();
		createToolbar();
		createStatusBar();

		setWindowTitle(tr("Omni"));
		resize(1000, 800);

		preferences = NULL;

		isProjectOpen = false;
		omniInspector = NULL;
		undoView = NULL;

		// prog_bar = new QProgressBar(statusBar());
		// statusBar()->addWidget(prog_bar);

		recentFiles.loadRecentlyUsedFilesListFromFS();

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}

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

		if (fileName == NULL)
			return;

		fileName = OmProject::New( fileName );
		recentFiles.addFile( fileName );

		setWindowTitle(tr("Omni - ") + fileName);

		isProjectOpen = true;

		updateComboBoxes();
		updateKeyShortcuts();

		OmStateManager::Instance()->SetViewSliceMin(XY_VIEW, Vector2 < float >(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMin(XZ_VIEW, Vector2 < float >(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMin(YZ_VIEW, Vector2 < float >(0.0, 0.0));

		OmStateManager::Instance()->SetViewSliceMax(XY_VIEW, Vector2 < float >(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMax(XZ_VIEW, Vector2 < float >(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMax(YZ_VIEW, Vector2 < float >(0.0, 0.0));

		OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, 0.0);
		OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, 0.0);
		OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, 0.0);

		OmStateManager::Instance()->SetZoomLevel(Vector2 < int >(0, 10));
		OmStateManager::Instance()->SetPanDistance(XY_VIEW, Vector2 < int >(0, 0));
		OmStateManager::Instance()->SetPanDistance(XZ_VIEW, Vector2 < int >(0, 0));
		OmStateManager::Instance()->SetPanDistance(YZ_VIEW, Vector2 < int >(0, 0));

		createDockWindows();

		setupToolbarInitially();

		openInspector();

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::showEditPreferencesDialog()
{
	if (!isProjectOpen) {
		return;
	}
	
	if (preferences) {
		preferences->close();
		delete preferences;
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
		delete preferences;
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
		if (!isProjectOpen) {
			return;
		}

		OmChannel & added_channel = OmVolume::AddChannel();

		if (omniInspector) {
			omniInspector->addToVolume(&added_channel, CHANNEL);
		}
	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::addSegmentationToVolume()
{
	try {
		if (!isProjectOpen) {
			return;
		}

		OmSegmentation & added_segmentation = OmVolume::AddSegmentation();

		if (omniInspector) {
			omniInspector->addToVolume(&added_segmentation, SEGMENTATION);
		}
	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

/* returns false if New/Open action is cancelled by user */
bool MainWindow::closeProjectIfOpen()
{

	if (!isProjectOpen) {
		return true;
	}

	if (!checkForSave()) {
		return false;
	}

	isProjectOpen = false;

	// get rid of QDockWidget that may contain Inspector, History, View, etc widgets        
	QDockWidget *dockwidget = this->findChild < QDockWidget * >();
	while (dockwidget != 0) {
		delete dockwidget;
		dockwidget = this->findChild < QDockWidget * >();
	}

	if (preferences) {
		preferences->close();
		delete preferences;
		preferences = NULL;
	}

	OmProject::Close();

	setWindowTitle(tr("Omni"));

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

		recentFiles.addFile( fileNameAndPath );

		isProjectOpen = true;

		updateComboBoxes();
		updateKeyShortcuts();

		OmStateManager::Instance()->SetViewSliceMin(XY_VIEW, Vector2 < float >(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMin(XZ_VIEW, Vector2 < float >(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMin(YZ_VIEW, Vector2 < float >(0.0, 0.0));

		OmStateManager::Instance()->SetViewSliceMax(XY_VIEW, Vector2 < float >(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMax(XZ_VIEW, Vector2 < float >(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMax(YZ_VIEW, Vector2 < float >(0.0, 0.0));

		SpaceCoord depth = OmVolume::NormToSpaceCoord( NormCoord(0.5, 0.5, 0.5));
		OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, depth.z);
		OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, depth.y);
		OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, depth.x);		



		//OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, 0.0);
		//OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, 0.0);
		//OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, 0.0);

		OmStateManager::Instance()->SetZoomLevel(Vector2 < int >(0, 10));
		OmStateManager::Instance()->SetPanDistance(XY_VIEW, Vector2 < int >(0, 0));
		OmStateManager::Instance()->SetPanDistance(XZ_VIEW, Vector2 < int >(0, 0));
		OmStateManager::Instance()->SetPanDistance(YZ_VIEW, Vector2 < int >(0, 0));

		createDockWindows();

		setupToolbarInitially();

		setWindowTitle(tr("Omni - ") + fileNameAndPath );
		openInspector();

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

		if (!isProjectOpen) {
			return;
		}

		omniInspector = new MyInspectorWidget( this );
		omniInspector->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Expanding );

		QDockWidget *dock = new QDockWidget(tr("Inspector"), this);
		dock->setAllowedAreas(Qt::AllDockWidgetAreas);

		omniInspector->setParent(dock);
		dock->setWidget(omniInspector);

		addDockWidget(Qt::TopDockWidgetArea, dock);
		windowMenu->addAction(dock->toggleViewAction());

		// TODO: fixme! (purcaro)
		connect(omniInspector, SIGNAL(addChannel()), this, SLOT(addChannelToVolume()));
		connect(omniInspector, SIGNAL(addSegmentation()), this, SLOT(addSegmentationToVolume()));

		connect(omniInspector, SIGNAL(triggerChannelView(OmId, OmId, OmId, ViewType)),
			this, SLOT(openChannelView(OmId, OmId, OmId, ViewType)));
		connect(omniInspector, SIGNAL(triggerSegmentationView(OmId, OmId, ViewType)),
			this, SLOT(openSegmentationView(OmId, OmId, ViewType)));

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::openUndoView()
{
	try {

		//debug("genone","opening undo history");
		undoView = new QUndoView();

		QDockWidget *dock = new QDockWidget(tr("History"), this);
		dock->setAllowedAreas(Qt::AllDockWidgetAreas);

		undoView->setParent(dock);
		dock->setWidget(undoView);

		addDockWidget(Qt::TopDockWidgetArea, dock);
		windowMenu->addAction(dock->toggleViewAction());

		undoView->setStack(OmStateManager::GetUndoStack());

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::open3dView()
{
	try {

		//debug("genone","MainWindow::open3dView()");

		if (isProjectOpen) {

			//debug("genone","opening 3d view");
			qtView3d = new OmView3d();

			QDockWidget *dock = new QDockWidget(tr("3D View"), this);
			dock->setAllowedAreas(Qt::AllDockWidgetAreas);

			qtView3d->setParent(dock);
			dock->setWidget(qtView3d);

			addDockWidget(Qt::TopDockWidgetArea, dock);
			windowMenu->addAction(dock->toggleViewAction());

			dock->setAttribute(Qt::WA_DeleteOnClose);
			//              //debug("genone","omni inspector delete on close: " << dock->testAttribute(Qt::WA_DeleteOnClose));
		}

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::openChannelView(OmId chan_id, OmId second_chan_id, OmId third_id, ViewType vtype)
{
	try {

		//debug("genone","MainWindow::openChannelView()");

		string name = OmVolume::GetChannel(chan_id).GetName();
		QString qname = QString::fromStdString(name);
		if (vtype == XY_VIEW)
			qname = qname + QString(" -- XY View");
		else if (vtype == XZ_VIEW)
			qname = qname + QString(" -- XZ View");
		else
			qname = qname + QString(" -- YZ View");

		QDockWidget *dock;

		OmView2d *qtView2d_current = new OmView2d(vtype, CHANNEL, chan_id, second_chan_id, third_id);
		dock = new QDockWidget(qname, this);
		dock->setAllowedAreas(Qt::AllDockWidgetAreas);

		qtView2d_current->setParent(dock);
		dock->setWidget(qtView2d_current);

		addDockWidget(Qt::TopDockWidgetArea, dock);
		windowMenu->addAction(dock->toggleViewAction());

		dock->setAttribute(Qt::WA_DeleteOnClose);

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}

}

void MainWindow::openSegmentationView(OmId primary_id, OmId secondary_id, ViewType vtype)
{
	try {
		OmView2d *qtView2d_current = new OmView2d(vtype, SEGMENTATION, primary_id, secondary_id);

		string name = OmVolume::GetSegmentation(primary_id).GetName();
		QString qname = QString::fromStdString(name);
		if (vtype == XY_VIEW)
			qname = qname + QString(" -- XY View");
		else if (vtype == XZ_VIEW)
			qname = qname + QString(" -- XZ View");
		else
			qname = qname + QString(" -- YZ View");

		QDockWidget *dock = new QDockWidget(qname, this);
		dock->setAllowedAreas(Qt::AllDockWidgetAreas);

		qtView2d_current->setParent(dock);
		dock->setWidget(qtView2d_current);

		addDockWidget(Qt::TopDockWidgetArea, dock);
		windowMenu->addAction(dock->toggleViewAction());

		dock->setAttribute(Qt::WA_DeleteOnClose);

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::updateKeyShortcuts()
{
	try {

		/*
		   string keystring = OmPreferences::GetString(OM_PREF_GUI_VIEW_CENTER_FORWARD_STR);

		   3) Constructing a QKeySequence from the user-specified key shortcut (do this for each key shortcut string retrieved in step 2)

		   QKeySequence matchSequence = QKeySequence::fromString(QString::fromStdString(keystring));
		 */

		string undoString = OmPreferences::GetString(OM_PREF_GUI_UNDO_STR);
		string redoString = OmPreferences::GetString(OM_PREF_GUI_REDO_STR);

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	try {

		// QMainWindow::saveState() and restoreState()

		if (isProjectOpen)
			if (!checkForSave())
				event->ignore();

		//delete exceptionMessage;

		//if(undoView)
		//delete undoView;
		// OmProject::Close();
	} catch(OmException & e) {
		spawnErrorDialog(e);
	}

}

void MainWindow::AlertNotifyEvent(OmAlertEvent * event)
{
	//debug("genone","MainWindow::AlertNotifyEvent");

	try {
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdString(event->GetText()));
		msgBox.setInformativeText(QString::fromStdString(event->GetMoreText()));
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
		msgBox.setDefaultButton(QMessageBox::Yes);
		int ret = msgBox.exec();

		switch (ret) {
		case QMessageBox::Yes:
			break;
		case QMessageBox::No:
			throw OmFormatException("Invalid data format.");
		default:
			// should never be reached
			break;
		}
	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::ProgressShow(OmProgressEvent * event)
{
	try {

		//      QProgressDialog progress("Copying files...", "Abort Copy", 0, numFiles, this);
		//      progress.setWindowModality(Qt::WindowModal);

		/*
		   int GetText() { return mText; }
		   int GetMinimum() { return mMinimum; }
		   int GetMaximum() { return mMaximum; }
		   int GetValue() { return mValue; }
		 */
		//debug("genone","MainWindow::ProgressShow");

//              prog_dialog = new QProgressDialog(QString::fromStdString(event->GetText()),
//                                                                                QString("Cancel"),
//                                                                                event->GetMinimum(),
//                                                                                event->GetMaximum(),
//                                                                                this);

		prog_dialog.reset();
		prog_dialog.setLabelText(QString::fromStdString(event->GetText()));
		prog_dialog.setCancelButtonText(QString("Cancel"));
		prog_dialog.setMinimum(event->GetMinimum());
		prog_dialog.setMaximum(event->GetMaximum());
		//prog_dialog->setParent(this);
		prog_dialog.setWindowModality(Qt::WindowModal);
		prog_dialog.setValue(event->GetValue());
		OmProgressEvent::SetWasCanceled(false);

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::ProgressHide(OmProgressEvent * event)
{
	try {

		//prog_dialog->cancel();
		//debug("genone","MainWindow::ProgressHide");
		//debug("FIXME", << "min: " << event->GetMinimum() << endl;
		//debug("FIXME", << "max: " << event->GetMaximum() << endl;
		//debug("FIXME", << "val: " << event->GetValue() << endl;
		prog_dialog.reset();
	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::ProgressRangeEvent(OmProgressEvent * event)
{
	try {

		/*
		   QProgressDialog progress("Copying files...", "Abort Copy", 0, numFiles, this);
		   progress.setWindowModality(Qt::WindowModal);

		   for (int i = 0; i < numFiles; i++) {
		   progress.setValue(i);

		   if (progress.wasCanceled())
		   break;
		   //... copy one file
		   }
		   progress.setValue(numFiles);
		 */

		//debug("genone","ProgressRangeEvent");
		//debug("FIXME", << "min: " << event->GetMinimum() << endl;
		//debug("FIXME", << "max: " << event->GetMaximum() << endl;
		//debug("FIXME", << "val: " << event->GetValue() << endl;

		prog_dialog.setRange(event->GetMinimum(), event->GetMaximum());

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::ProgressValueEvent(OmProgressEvent * event)
{
	try {

		//debug("genone","ProgressValueEvent");

		//check for cancel
		OmProgressEvent::SetWasCanceled(prog_dialog.wasCanceled());
		//ignore if progress bar is canceled
		if (prog_dialog.wasCanceled())
			return;
		//update value
		prog_dialog.setValue(event->GetValue());

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::ProgressIncrementEvent(OmProgressEvent * event)
{
	try {

		//debug("genone","ProgressIncrementEvent");
		//debug("FIXME", << "min: " << event->GetMinimum() << endl;
		//debug("FIXME", << "max: " << event->GetMaximum() << endl;
		//debug("FIXME", << "val: " << event->GetValue() << endl;
		//      int val = prog_bar->value();
		//      prog_bar->setValue(val + 1);

		//check for cancel
		OmProgressEvent::SetWasCanceled(prog_dialog.wasCanceled());
		//ignore if progress bar is canceled
		if (prog_dialog.wasCanceled())
			return;
		//update value
		prog_dialog.setValue(prog_dialog.value() + 1);

	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::updateStatusBar()
{

}

void MainWindow::createActions()
{
	// Menubar
	// File
	newAct = new QAction(tr("&New Project..."), this);
	newAct->setShortcut(tr("Ctrl+N"));
	newAct->setStatusTip(tr("Begin a new project"));
	connect(newAct, SIGNAL(triggered()), this, SLOT(newProject()));

	openAct = new QAction(tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setStatusTip(tr("Open an existing project"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(openProject()));

	closeAct = new QAction(tr("&Close"), this);
	closeAct->setStatusTip(tr("Close current project"));
	connect(closeAct, SIGNAL(triggered()), this, SLOT(closeProject()));

	saveAct = new QAction(tr("&Save"), this);
	saveAct->setShortcut(tr("Ctrl+S"));
	saveAct->setStatusTip(tr("Saves the current project"));
	connect(saveAct, SIGNAL(triggered()), this, SLOT(saveProject()));

	for (int i = 0; i < recentFiles.getMaxNumberOfRecentlyUsedFilesToDisplay(); i++) {
		recentFiles.recentFileActs[i] = new QAction(this);
		recentFiles.recentFileActs[i]->setVisible(false);
		connect(recentFiles.recentFileActs[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	}

	quitAct = new QAction(tr("&Quit"), this);
	quitAct->setShortcut(tr("Ctrl+Q"));
	quitAct->setStatusTip(tr("Quit the application"));
	connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

	// Edit
	editLocalPreferencesAct = new QAction(tr("&Local Preferences"), this);
	connect(editLocalPreferencesAct, SIGNAL(triggered()), this, SLOT(showEditLocalPreferencesDialog()));

	editPreferencesAct = new QAction(tr("&Project Preferences"), this);
	connect(editPreferencesAct, SIGNAL(triggered()), this, SLOT(showEditPreferencesDialog()));

	// Project
	addChannelAct = new QAction(tr("Add &Channel"), this);
	addChannelAct->setShortcut(tr("Ctrl+Shift+C"));
	addChannelAct->setStatusTip(tr("Adds a channe to the current project"));
	connect(addChannelAct, SIGNAL(triggered()), this, SLOT(addChannelToVolume()));

	addSegmentationAct = new QAction(tr("Add &Segmentation"), this);
	addSegmentationAct->setShortcut(tr("Ctrl+Shift+S"));
	addSegmentationAct->setStatusTip(tr("Adds a volume to the current project"));
	connect(addSegmentationAct, SIGNAL(triggered()), this, SLOT(addSegmentationToVolume()));

	// Tools
	openOmniInspector = new QAction(tr("&Inspector"), this);
	openOmniInspector->setShortcut(tr("Ctrl+I"));
	openOmniInspector->setStatusTip(tr("Opens the Omni Inspector"));
	connect(openOmniInspector, SIGNAL(triggered()), this, SLOT(openInspector()));

	openUndoViewAct = new QAction(tr("&History"), this);
	openUndoViewAct->setStatusTip(tr("Opens the Undo History"));
	connect(openUndoViewAct, SIGNAL(triggered()), this, SLOT(openUndoView()));

	// Window
	open3DAct = new QAction(tr("Open &3D View"), this);
	open3DAct->setShortcut(tr("Ctrl+3"));
	open3DAct->setStatusTip(tr("Opens the 3D view"));
	connect(open3DAct, SIGNAL(triggered()), this, SLOT(open3dView()));
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addSeparator();
	fileMenu->addAction(closeAct);
	fileMenu->addAction(saveAct);
	fileMenu->addSeparator();
	for (int i = 0; i < recentFiles.getMaxNumberOfRecentlyUsedFilesToDisplay(); i++) {
		fileMenu->addAction(recentFiles.recentFileActs[i]);
	}
	fileMenu->addSeparator();
	fileMenu->addAction(quitAct);

	editMenu = menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(editLocalPreferencesAct);
	editMenu->addAction(editPreferencesAct);

	projectMenu = menuBar()->addMenu(tr("&Project"));
	projectMenu->addAction(addChannelAct);
	projectMenu->addAction(addSegmentationAct);

	toolMenu = menuBar()->addMenu(tr("&Tools"));
	toolMenu->addAction(openOmniInspector);
	toolMenu->addAction(openUndoViewAct);

	windowMenu = menuBar()->addMenu(tr("&Window"));
	windowMenu->addAction(open3DAct);
}

void MainWindow::createStatusBar()
{
	// statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createDockWindows()
{

	//      QDockWidget *spacerdock = new QDockWidget(tr("Spacer"), this);
	//      spacerdock->setAllowedAreas(Qt::AllDockWidgetAreas);
	//      
	//      QWidget *spacer = new QWidget;
	//      
	//      spacer->setParent(spacerdock);
	//      spacerdock->setWidget(spacer);
	//      
	//      //      splitDockWidget(mydock, spacerdock, Qt::Vertical);
	//      
	//      addDockWidget(Qt::AllDockWidgetAreas, spacerdock);
	//      viewMenu->addAction(spacerdock->toggleViewAction());

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

	QString errorMessage = e.GetType() + ": " + e.GetName() + ". " + e.GetMessage();
	exceptionMessage->showMessage(errorMessage);
	printf("something bad happened in %s:, \n\t%s\n", __FUNCTION__, qPrintable(errorMessage) );
}

////////////////////////////////////////////////////////////
// Toolbars
////////////////////////////////////////////////////////////
void MainWindow::createToolbar()
{
	createToolbarActions();
	addToolbars();
	setupSegmentationBoxes();
}

void MainWindow::createToolbarActions()
{
	modifyAct = new QAction(tr("&Modify Mode"), this);
	modifyAct->setStatusTip(tr("Switches to Modify Mode"));
	connect(modifyAct, SIGNAL(triggered(bool)), this, SLOT(ChangeModeModify(bool)));
	modifyAct->setCheckable(true);

	toolbarSelectAct = new QAction(tr("&Select"), this);
	toolbarSelectAct->setStatusTip(tr("Switches to Object Selection Mode"));
	connect(toolbarSelectAct, SIGNAL(triggered(bool)), this, SLOT(toolbarSelect(bool)));
	toolbarSelectAct->setCheckable(true);

	toolbarCrosshairAct = new QAction(tr("&Crosshair"), this);
	toolbarCrosshairAct->setStatusTip(tr("Switches on Crosshairs"));
	connect(toolbarCrosshairAct, SIGNAL(triggered(bool)), this, SLOT(toolbarCrosshair(bool)));
	toolbarCrosshairAct->setCheckable(true);

	toolbarPanAct = new QAction(tr("&Pan"), this);
	toolbarPanAct->setStatusTip(tr("Switches to Pan Mode"));
	connect(toolbarPanAct, SIGNAL(triggered(bool)), this, SLOT(toolbarPan(bool)));
	toolbarPanAct->setCheckable(true);

	toolbarZoomAct = new QAction(tr("&Zoom"), this);
	toolbarZoomAct->setStatusTip(tr("Switches to Zoom Mode"));
	connect(toolbarZoomAct, SIGNAL(triggered(bool)), this, SLOT(toolbarZoom(bool)));
	toolbarZoomAct->setCheckable(true);

	toolbarVoxelizeAct = new QAction(tr("&Voxelizer"), this);
	toolbarVoxelizeAct->setStatusTip(tr("Switches to Voxel Mode"));
	connect(toolbarVoxelizeAct, SIGNAL(triggered(bool)), this, SLOT(toolbarVoxelize(bool)));
	toolbarVoxelizeAct->setCheckable(true);

	toolbarBrushAct = new QAction(tr("&Brush"), this);
	toolbarBrushAct->setStatusTip(tr("Switches to Voxel Add Mode"));
	connect(toolbarBrushAct, SIGNAL(triggered(bool)), this, SLOT(toolbarBrush(bool)));
	toolbarBrushAct->setCheckable(true);

	toolbarEraserAct = new QAction(tr("&Eraser"), this);
	toolbarEraserAct->setStatusTip(tr("Switches to Voxel Subtraction Mode"));
	connect(toolbarEraserAct, SIGNAL(triggered(bool)), this, SLOT(toolbarEraser(bool)));
	toolbarEraserAct->setCheckable(true);

	toolbarFillAct = new QAction(tr("&Fill"), this);
	toolbarFillAct->setStatusTip(tr("Switches to Fill Mode"));
	connect(toolbarFillAct, SIGNAL(triggered(bool)), this, SLOT(toolbarFill(bool)));
	toolbarFillAct->setCheckable(true);
}

void MainWindow::addToolbars()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(saveAct);

	systemToolBar = addToolBar(tr("Mode"));
	systemToolBar->addAction(modifyAct);

	navigateToolBar = addToolBar(tr("Navigate"));
	navigateToolBar->addAction(toolbarSelectAct);
	navigateToolBar->addAction(toolbarCrosshairAct);
	navigateToolBar->addAction(toolbarPanAct);
	navigateToolBar->addAction(toolbarZoomAct);
	navigateToolBar->addAction(toolbarVoxelizeAct);

	toolToolBar = addToolBar(tr("Tools"));
	toolToolBar->addAction(toolbarBrushAct);
	toolToolBar->addAction(toolbarEraserAct);
	toolToolBar->addAction(toolbarFillAct);
}

void MainWindow::setupToolbarInitially()
{
	resetTools(NAVIGATION_SYSTEM_MODE);

	OmStateManager::SetSystemMode(NAVIGATION_SYSTEM_MODE);
	OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
}

void MainWindow::ChangeModeModify(const bool checked)
{
	try {
		if (checked) {
			OmStateManager::SetSystemMode(EDIT_SYSTEM_MODE);
			resetTools(EDIT_SYSTEM_MODE);
		} else {
			OmStateManager::SetSystemMode(NAVIGATION_SYSTEM_MODE);
			resetTools(NAVIGATION_SYSTEM_MODE);
		}

		OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::toolbarToolChange(const bool checked, QAction * tool, const OmToolMode mode)
{
	if (checked) {
		resetViewTools();

		switch (OmStateManager::GetSystemMode()) {
		case (NAVIGATION_SYSTEM_MODE):
			resetModifyTools(false);
			break;
		case (EDIT_SYSTEM_MODE):
			resetModifyTools(true);
			break;
		default:
			break;
		}

		tool->setChecked(true);
		OmStateManager::SetToolMode(mode);
		OmEventManager::PostEvent(new OmToolModeEvent(OmToolModeEvent::TOOL_MODE_CHANGE));
	}
}

// view tools
void MainWindow::toolbarSelect(const bool checked)
{
	toolbarToolChange(checked, toolbarSelectAct, SELECT_MODE); 
}

void MainWindow::toolbarCrosshair(const bool checked)
{
	toolbarToolChange(checked, toolbarCrosshairAct, CROSSHAIR_MODE);
}

void MainWindow::toolbarPan(const bool checked)
{
	toolbarToolChange(checked, toolbarPanAct, PAN_MODE);
}

void MainWindow::toolbarZoom(const bool checked)
{
	toolbarToolChange(checked, toolbarZoomAct, ZOOM_MODE);
}

void MainWindow::toolbarVoxelize(const bool checked)
{
	toolbarToolChange(checked, toolbarVoxelizeAct, VOXELIZE_MODE);
}

// modify tools
void MainWindow::toolbarBrush(const bool checked)
{
	toolbarToolChange(checked, toolbarBrushAct, ADD_VOXEL_MODE);
}

void MainWindow::toolbarEraser(const bool checked)
{
	toolbarToolChange(checked, toolbarEraserAct, SUBTRACT_VOXEL_MODE);
}

void MainWindow::toolbarFill(const bool checked)
{
	toolbarToolChange(checked, toolbarFillAct, SELECT_VOXEL_MODE);
}

void MainWindow::resetTools(const OmSystemMode sys_mode)
{
	switch (sys_mode) {
	case (NAVIGATION_SYSTEM_MODE):
		resetModifyTools(false);
		toolbarPanAct->setChecked(true);
		OmStateManager::SetToolMode(PAN_MODE);
		break;
	case (EDIT_SYSTEM_MODE):
		resetModifyTools(true);
		break;
	default:
		break;
	}
}

void MainWindow::resetTool(QAction * tool, const bool enabled)
{
	tool->setChecked(false);
	tool->setEnabled(enabled);
}

void MainWindow::resetViewTools()
{
	resetTool(toolbarSelectAct, true);
	resetTool(toolbarCrosshairAct, true);
	resetTool(toolbarPanAct, true);
	resetTool(toolbarZoomAct, true);
	resetTool(toolbarVoxelizeAct, true);
}

void MainWindow::resetModifyTools(const bool enabled)
{
	resetTool(toolbarBrushAct, enabled);
	resetTool(toolbarEraserAct, enabled);
	resetTool(toolbarFillAct, enabled);
}

////////////////////////////////////////////////////////////
// Segmentation Combo Box in Toolbar

void MainWindow::setupSegmentationBoxes()
{
	selectSegmentationBox = new QComboBox();
	selectSegmentationBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	toolToolBar->addWidget(selectSegmentationBox);

	editColorButton = new QPushButton();
	editColorButton->setMaximumWidth(50);
	toolToolBar->addWidget(editColorButton);

	connect(selectSegmentationBox, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(changeSelection(int)));
}

void MainWindow::updateComboBoxes(const OmId segmentationID, const OmId segmentJustSelectedID)
{
	try {
		selectSegmentationBox->blockSignals(true);
		selectSegmentationBox->clear();
		editColorButton->setIcon(QIcon());

		int indexToSet = -1;
		int counter = 0;
		foreach( SegmentDataWrapper segH, OmVolume::GetSelectedSegmentIDs()) {
			QString comboBoxRowStr = segH.getSegmentationName() + "--" + segH.getName();
			selectSegmentationBox->addItem(comboBoxRowStr, qVariantFromValue(segH));

			// if we get none-zero segmenetation and segment IDs, select the segment 
			//  (in the correct segmentation) as the currently active combo box item
			if (segmentationID == segH.getSegmentationID()) {
				if (segmentJustSelectedID == segH.getID()) {
					indexToSet = counter;
				}
			}
			counter++;
		}
		if (indexToSet != -1) {
			selectSegmentationBox->setCurrentIndex(indexToSet);
		}
		selectSegmentationBox->update();
		selectSegmentationBox->blockSignals(false);
	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::SegmentObjectModificationEvent(OmSegmentEvent * event)
{
	try {
		if(event->getSender() == this) {
			//debug("gui", "%s...; skipping since i sent it! (%s)\n", __FUNCTION__, event->getComment().c_str());
			return;
		}

		const OmId segmentationID = event->GetModifiedSegmentationId();
		const OmId segmentJustSelectedID = event->GetSegmentJustSelectedID();
		updateComboBoxes(segmentationID, segmentJustSelectedID);
	} catch(OmException & e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::changeSelection(int segmentIndex)
{
	try {
		editColorButton->setIcon(QIcon());

		QVariant result = selectSegmentationBox->itemData(selectSegmentationBox->currentIndex());
		SegmentDataWrapper segH = result.value < SegmentDataWrapper > ();
		const OmId segmentationID = segH.getSegmentationID();
		const OmId segmentID = segH.getID();

		if( !OmVolume::IsSegmentationValid( segmentationID ) ){
			debug("gui", "invalid segmentation found; ID is %d\n", segmentationID );
			return;
		}
		OmSegmentation& segmentation = OmVolume::GetSegmentation(segmentationID);
		if( !segmentation.IsSegmentValid( segmentID ) ){
			debug("gui", "invalid segment found; ID is %d\n", segmentID );
			return;
		}
		
		OmSegmentEditor::SetEditSelection(segmentationID, segmentID);

		OmSegment& segment = segmentation.GetSegment(segmentID);
		const Vector3 < float >&color = segment.GetColor();
		
		QPixmap *pixm = new QPixmap(40, 30);
		QColor newcolor = qRgb(color.x * 255, color.y * 255, color.z * 255);
		pixm->fill(newcolor);
		
		editColorButton->setIcon(QIcon(*pixm));
		
		OmIds selected_segment_ids;
		selected_segment_ids.insert(segmentID);
		OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
							     segmentationID,
							     selected_segment_ids, 
							     segmentID, 
							     this,
							     "mainwindow"));
	} catch(OmException & e) {
		// We want to just ignore random voodoo that happened. Don't let the user know. MW.
		spawnErrorDialog(e);
	}
}

void MainWindow::SystemModeChangeEvent(OmSystemModeEvent * event)
{
	debug("gui", "hi from %s\n", __FUNCTION__);

	switch (OmStateManager::GetToolMode()) {
	case SELECT_MODE:
		toolbarToolChange(true, toolbarSelectAct, SELECT_MODE); 
		break;
	case PAN_MODE:
		toolbarToolChange(true, toolbarPanAct, PAN_MODE);
		break;
	case CROSSHAIR_MODE:
		toolbarToolChange(true, toolbarCrosshairAct, CROSSHAIR_MODE);
		break;
	case ZOOM_MODE:
		toolbarToolChange(true, toolbarZoomAct, ZOOM_MODE);
		break;
	case ADD_VOXEL_MODE:
		toolbarToolChange(true, toolbarBrushAct, ADD_VOXEL_MODE);
		break;
	case SUBTRACT_VOXEL_MODE:
		toolbarToolChange(true, toolbarEraserAct, SUBTRACT_VOXEL_MODE);
		break;
	case SELECT_VOXEL_MODE:
		toolbarToolChange(true, toolbarFillAct, SELECT_VOXEL_MODE);
		break;
	case VOXELIZE_MODE:
		toolbarToolChange(true, toolbarVoxelizeAct, VOXELIZE_MODE);
		break;
	case FILL_MODE:
		// TODO???
		break;
	}
}

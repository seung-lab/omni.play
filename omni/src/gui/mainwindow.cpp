#include <QList>
#include <QMultiMap>
#include <QUndoStack>
#include <QUndoView>

#include "view2d/omTile.h"
#include "view2d/omCachingTile.h"
#include "view2d/omTextureID.h"

#include "mainwindow.h"
#include "recentFileList.h"

#include "view2d/omView2d.h"
#include "view2d/domView2d.h"
#include "view3d/omView3d.h"
#include "system/omProject.h"
#include "volume/omChannel.h"
#include "volume/omVolume.h"

#include "myInspectorWidget.h"

#include "volume/omChannel.h"
#include "volume/omSegmentation.h"

#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"

#include "system/omStateManager.h"
#include "system/omEventManager.h"
#include "system/events/omSystemModeEvent.h"
#include "system/events/omToolModeEvent.h"
#include "system/omSystemTypes.h"
#include "segment/omSegmentEditor.h"

#include "system/omException.h"

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs=boost::filesystem;

#include <boost/tuple/tuple_comparison.hpp>
using boost::tuple;

#define DEBUG 0

Q_DECLARE_METATYPE( SegmentIDhelper );

MainWindow::MainWindow()
: prog_dialog(this)
{
	exceptionMessage = new QErrorMessage(this);
	
	try {
		
		setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
		setAnimated (false);
		
		setFocusPolicy(Qt::ClickFocus);
		
		
		loadingDock = new QFrame;
		loadingDock->setFrameStyle(QFrame::Box | QFrame::Raised);
		
		QGridLayout *dockLayout = new QGridLayout;
		
		loadingDock->setLayout(dockLayout);
		
		
		createActions();
		createMenus();
		//		createToolBarsNew();
		createToolBars();
		createStatusBar();
		
		connect(editAct, SIGNAL(toggled(bool)), selectSegmentationBox, SLOT(setEnabled(bool)));
 		//connect(editAct, SIGNAL(toggled(bool)), selectSegmentBox, SLOT(setEnabled(bool)));
		connect(editAct, SIGNAL(toggled(bool)), editColorButton, SLOT(setEnabled(bool)));
		
		connect(selectSegmentationBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeSelection(int)));
		
		setWindowTitle(tr("Omni"));
		resize(1000,800);
		
		isProjectOpen = false;
		omniInspector = NULL;
		undoView = NULL;
		
		iSentIt = false;
		// prog_bar = new QProgressBar(statusBar());
		// statusBar()->addWidget(prog_bar);

		recentFiles.loadRecentlyUsedFilesListFromFS();
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
	
}

// Creates a new project
// Prompts the user for a directory in which to create the new project
// Internally, this sets the current working directory where the Volume will be serialized
void MainWindow::newProject()
{
	try {
		
		if( !closeProjectIfOpen() ){
		  return;
		}
				
		QString fileName = QFileDialog::getSaveFileName(this, tr("New Project")); 
		
		if(fileName == NULL)
			return;
		
		DOUT("New Project");
		QString fname = fileName.section('/', -1);
		QString dpath = fileName.remove(fname);
		
		if(! fname.endsWith(".omni"))
			fname.append(".omni");
		
		OmProject::New(dpath.toStdString(), fname.toStdString());
		
		recentFiles.addFile( fname, dpath );

		setWindowTitle(tr("Omni - ") + fname );

		isProjectOpen = true;
		
		updateComboBoxes();
		updateKeyShortcuts();
		
		OmStateManager::Instance()->SetViewSliceMin(XY_VIEW, Vector2<float>(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMin(XZ_VIEW, Vector2<float>(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMin(YZ_VIEW, Vector2<float>(0.0, 0.0));
		
		OmStateManager::Instance()->SetViewSliceMax(XY_VIEW, Vector2<float>(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMax(XZ_VIEW, Vector2<float>(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMax(YZ_VIEW, Vector2<float>(0.0, 0.0));
		
		OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, 0.0);
		OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, 0.0);
		OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, 0.0);
		
		OmStateManager::Instance()->SetZoomLevel(Vector2<int>(0, 10));
		OmStateManager::Instance()->SetPanDistance(XY_VIEW, Vector2<int>(0, 0));
		OmStateManager::Instance()->SetPanDistance(XZ_VIEW, Vector2<int>(0, 0));
		OmStateManager::Instance()->SetPanDistance(YZ_VIEW, Vector2<int>(0, 0));
		
		createDockWindows();
		
		//navigateAct->trigger();
		
		selectSegmentationBox->setEnabled(false);
		//		selectSegmentBox->setEnabled(false);
		editColorButton->setEnabled(false);

		setWindowTitle(tr("Omni - ") + fname );
		
		openInspector();
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::addChannelToVolume()
{
	try {
		if( !isProjectOpen ) {
			return;
		}
			
		OmChannel& added_channel = OmVolume::AddChannel();
		
		if(omniInspector) {
		  omniInspector->addToVolume(&added_channel, CHANNEL);
		}
	} catch (OmException &e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::addSegmentationToVolume()
{
	try {
		if( !isProjectOpen ) {
			return;
		}
		
		OmSegmentation& added_segmentation = OmVolume::AddSegmentation();
		
		if(omniInspector) {
			omniInspector->addToVolume(&added_segmentation, SEGMENTATION);
		} 
	} catch (OmException &e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::buildAll()
{
	try {
		if( !isProjectOpen ) {
			return;
		}
		
		const set<OmId> channelIDs = OmVolume::GetValidChannelIds();
		const set<OmId> segmentationIDs = OmVolume::GetValidSegmentationIds();
		
		QFuture<void> finalChanFuture;
		set<OmId>::iterator chan_it;
		for( chan_it=channelIDs.begin(); chan_it != channelIDs.end(); chan_it++ ) {
			OmChannel &current_channel = OmVolume::GetChannel(*chan_it);
			DOUT("building channel");
			//			prog_bar->setValue(ctr);
			//			progress.setValue(ctr);
			
			//			if (progress.wasCanceled())
			//				return;
			
			extern void channel_build(OmChannel *current_channel);
			QFuture<void> chanFuture = QtConcurrent::run(channel_build, &current_channel);
			finalChanFuture = chanFuture;
			// current_channel.BuildVolumeData();
			
			//			future.waitForFinished();
		}
		
		finalChanFuture.waitForFinished();
		
		set<OmId>::iterator seg_it;
		
		QFuture<void> finalFuture;
		for( seg_it=segmentationIDs.begin(); seg_it != segmentationIDs.end(); seg_it++ ) {
		  
		  	OmSegmentation &current_seg = OmVolume::GetSegmentation(*seg_it);
			DOUT("building segmentation");
			
			//			prog_bar->setValue(ctr);
			//			progress.setValue(ctr);
			//
			//			if (progress.wasCanceled())
			//				return;
			extern void seg_build(OmSegmentation *current_seg);
			extern void mesh_build(OmSegmentation *current_seg, QFuture<void> &last_future);
			
			QFuture<void> segFuture = QtConcurrent::run(seg_build, &current_seg);
			QFuture<void> meshFuture = QtConcurrent::run(mesh_build, &current_seg, segFuture);
			finalFuture = meshFuture;
		}
		
		//		statusBar()->removeWidget(prog_bar);
		//		delete prog_bar;
		
		finalFuture.waitForFinished();
		
		if(omniInspector) {
			omniInspector->refreshWidgetData();
		}
	} catch (OmException &e) {
		spawnErrorDialog(e);
	}
}

void channel_build(OmChannel *current_channel) {
	//	if(last_future.isRunning())
	//		last_future.waitForFinished();
	current_channel->BuildVolumeData();
}

void seg_build(OmSegmentation *current_seg) {
	current_seg->BuildVolumeData();
}

void mesh_build(OmSegmentation *current_seg, QFuture<void> &last_future) {
	if(last_future.isRunning())
		last_future.waitForFinished();
	current_seg->BuildMeshData();
}

/* returns false if New/Open action is cancelled by user */
bool MainWindow::closeProjectIfOpen() {

	if( !isProjectOpen ){
		return true;
	}
	
	if ( !checkForSave() ){
		return false;
	}

	isProjectOpen = false;

	// get rid of QDockWidget that may contain Inspector, History, View, etc widgets	
	QDockWidget *dockwidget = this->findChild<QDockWidget *>();
	while(dockwidget != 0)
	{
		delete dockwidget;
		dockwidget = this->findChild<QDockWidget *>();
	}

	OmProject::Close();

	setWindowTitle(tr("Omni"));

	return true;
}


void MainWindow::openRecentFile()
{
	QAction *action = qobject_cast<QAction *>(sender());

	if ( !action ) {
		return;
	}

	if( !closeProjectIfOpen() ){
		return;
	}
	
	openProject( action->data().toString() );
}


void MainWindow::openProject()
{
	try {
		if( !closeProjectIfOpen() ){
		  return;
		}
		
		// Opens an existing project
		// Prompts the user for their project file
		// Internally, this is the file that has Volume serialized
		
		QString fileNameAndPath = QFileDialog::getOpenFileName(this,
				tr("Open Project"), "", tr("Project File (*.omni)"));
		
		if(fileNameAndPath == NULL)
			return;
		
		//open project at fpath
		openProject(fileNameAndPath);
	} catch (OmException &e) {
		spawnErrorDialog(e);
	}
}



void MainWindow::openProject( QString fileNameAndPath )
{
	try {
		QString fname = fileNameAndPath.section('/', -1);
		QString dpath = fileNameAndPath.remove(fname);
		openProject( fname, dpath );

	} catch (OmException &e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::openProject( QString fname, QString dpath )
{
	try {
		
		try {
		  OmProject::Load(dpath.toStdString(), fname.toStdString());
		} catch(...){
		  throw OmIoException("error during load of OmProject object");
		}

		recentFiles.addFile( fname, dpath );
		
		isProjectOpen = true;
		
		updateComboBoxes();
		updateKeyShortcuts();
		
		OmStateManager::Instance()->SetViewSliceMin(XY_VIEW, Vector2<float>(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMin(XZ_VIEW, Vector2<float>(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMin(YZ_VIEW, Vector2<float>(0.0, 0.0));
		
		OmStateManager::Instance()->SetViewSliceMax(XY_VIEW, Vector2<float>(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMax(XZ_VIEW, Vector2<float>(0.0, 0.0));
		OmStateManager::Instance()->SetViewSliceMax(YZ_VIEW, Vector2<float>(0.0, 0.0));
		
		OmStateManager::Instance()->SetViewSliceDepth(XY_VIEW, 0.0);
		OmStateManager::Instance()->SetViewSliceDepth(XZ_VIEW, 0.0);
		OmStateManager::Instance()->SetViewSliceDepth(YZ_VIEW, 0.0);
		
		OmStateManager::Instance()->SetZoomLevel(Vector2<int>(0, 10));
		OmStateManager::Instance()->SetPanDistance(XY_VIEW, Vector2<int>(0, 0));
		OmStateManager::Instance()->SetPanDistance(XZ_VIEW, Vector2<int>(0, 0));
		OmStateManager::Instance()->SetPanDistance(YZ_VIEW, Vector2<int>(0, 0));
		
		createDockWindows();
		
		navigateAct->trigger();
		
		selectSegmentationBox->setEnabled(false);
		//selectSegmentBox->setEnabled(false);
		editColorButton->setEnabled(false);

		setWindowTitle(tr("Omni - ") + fname );
		openInspector();
	} catch (OmException &e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::closeProject()
{
	try {
		closeProjectIfOpen();
		
	} catch (OmException &e) {
		spawnErrorDialog(e);
	}
}

bool MainWindow::saveProject()
{
	try {
		
		// Saves the current project
		// Does not prompt the user
		// Serializes Volume to the current working directory
		
		OmProject::Save();
	} catch (OmException &e) {
		spawnErrorDialog(e);
	}
}


void MainWindow::openInspector()
{ 
	try {
		
		if( !isProjectOpen ) {
			return;
		}
			
		omniInspector = new MyInspectorWidget();
		
		QDockWidget *dock = new QDockWidget(tr("Inspector"), this);
		dock->setAllowedAreas(Qt::AllDockWidgetAreas);
		
		omniInspector->setParent(dock);
		dock->setWidget(omniInspector);
		
		addDockWidget(Qt::TopDockWidgetArea, dock);
		windowMenu->addAction(dock->toggleViewAction());
		
		connect(omniInspector, SIGNAL(treeDataChanged()), 
			this, SLOT(updateComboBoxes()));
		connect(omniInspector, SIGNAL(addChannel()), 
			this, SLOT(addChannelToVolume()));
		connect(omniInspector, SIGNAL(addSegmentation()), 
			this, SLOT(addSegmentationToVolume()));
		
		connect(omniInspector, SIGNAL(triggerChannelView(OmId, OmId, OmId, ViewType)), 
			this, SLOT(openChannelView(OmId, OmId, OmId, ViewType)));
		connect(omniInspector, SIGNAL(triggerSegmentationView(OmId, OmId, ViewType)), 
			this, SLOT(openSegmentationView(OmId, OmId, ViewType)));
		
		dock->setAttribute(Qt::WA_DeleteOnClose);

	} catch (OmException &e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::openUndoView()
{
	try {
		
		DOUT("opening undo history");
		undoView = new QUndoView();
		
		QDockWidget *dock = new QDockWidget(tr("History"), this);
		dock->setAllowedAreas(Qt::AllDockWidgetAreas);
		
		undoView->setParent(dock);
		dock->setWidget(undoView);
		
		addDockWidget(Qt::TopDockWidgetArea, dock);
		windowMenu->addAction(dock->toggleViewAction());
		
		undoView->setStack(OmStateManager::GetUndoStack());
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::open3dView()
{
	try {
		
		DOUT("MainWindow::open3dView()");
		
		if(	isProjectOpen ) {
			
			DOUT("opening 3d view");
			qtView3d = new OmView3d();
			
			
			QDockWidget *dock = new QDockWidget(tr("3D View"), this);
			dock->setAllowedAreas(Qt::AllDockWidgetAreas);
			
			qtView3d->setParent(dock);
			dock->setWidget(qtView3d);
			
			addDockWidget(Qt::TopDockWidgetArea, dock);
			windowMenu->addAction(dock->toggleViewAction());
			
			dock->setAttribute(Qt::WA_DeleteOnClose);
			//		DOUT("omni inspector delete on close: " << dock->testAttribute(Qt::WA_DeleteOnClose));
		}
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::openChannelView(OmId chan_id, OmId second_chan_id, OmId third_id, ViewType vtype)
{ 
	try {
		
		DOUT("MainWindow::openChannelView()");

		string name = OmVolume::GetChannel(chan_id).GetName();
		QString qname = QString::fromStdString(name);
		if(vtype == XY_VIEW)
			qname = qname + QString(" -- XY View");
		else if(vtype == XZ_VIEW)
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
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
	
}

void MainWindow::openSegmentationView(OmId primary_id, OmId secondary_id, ViewType vtype)
{
	try {
		
		DOUT("MainWindow::openSegmentationView()");
			
#pragma mark <TODO: Fix this overlay> 
		OmView2d *qtView2d_current = new OmView2d(vtype, SEGMENTATION, primary_id, secondary_id);
			
		string name = OmVolume::GetSegmentation(primary_id).GetName();
		QString qname = QString::fromStdString(name);
		if(vtype == XY_VIEW)
			qname = qname + QString(" -- XY View");
		else if(vtype == XZ_VIEW)
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

		
	} catch (OmException &e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::ChangeModeNavigation(bool checked)
{
	try {
		
		if(checked) {
			editAct->setChecked(false);
			editAddAct->setChecked(false);
			editSubtractAct->setChecked(false);
			editSelectAct->setChecked(false);
			
			OmStateManager::SetSystemMode(NAVIGATION_SYSTEM_MODE);
			OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
			
		}
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}


void MainWindow::ChangeModeEdit(bool checked)
{
	try {
		
		if(checked) {
			navigateAct->setChecked(false);
			
			editAddAct->trigger();
			
			OmStateManager::SetSystemMode(EDIT_SYSTEM_MODE);
			OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
		}
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::ChangeToolZoom(bool checked)
{
	try {
		
		if(checked) {
			
			editAddAct->setChecked(false);
			editSubtractAct->setChecked(false);
			editSelectAct->setChecked(false);
			panAct->setChecked(false);
			
			OmStateManager::SetToolMode(ZOOM_MODE);
			OmEventManager::PostEvent(new OmToolModeEvent(OmToolModeEvent::TOOL_MODE_CHANGE));
			
		}
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::ChangeToolPan(bool checked)
{
	try {
		
		if(checked) {
			
			editAddAct->setChecked(false);
			editSubtractAct->setChecked(false);
			editSelectAct->setChecked(false);
			zoomAct->setChecked(false);
			
			OmStateManager::SetToolMode(ZOOM_MODE);
			OmEventManager::PostEvent(new OmToolModeEvent(OmToolModeEvent::TOOL_MODE_CHANGE));
			
		}
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
	
}

void MainWindow::ChangeToolAdd(bool checked)
{
	try {
		
		if(checked) {
			
			navigateAct->setChecked(false);
			editAct->setChecked(true);
			zoomAct->setChecked(false);
			panAct->setChecked(false);
			editSubtractAct->setChecked(false);
			editSelectAct->setChecked(false);
			
			OmStateManager::SetToolMode(ADD_VOXEL_MODE);
			//cout << "set tool mode add voxel mode" << endl;
			OmEventManager::PostEvent(new OmToolModeEvent(OmToolModeEvent::TOOL_MODE_CHANGE));
			
		}
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::ChangeToolSubtract(bool checked)
{ 
	try {
		
		if(checked) {
			
			navigateAct->setChecked(false);
			editAct->setChecked(true);
			zoomAct->setChecked(false);
			panAct->setChecked(false);
			editAddAct->setChecked(false);
			editSelectAct->setChecked(false);
			
			OmStateManager::SetToolMode(SUBTRACT_VOXEL_MODE);
			OmEventManager::PostEvent(new OmToolModeEvent(OmToolModeEvent::TOOL_MODE_CHANGE));
			
		}
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
	
}

void MainWindow::ChangeToolSelect(bool checked)
{
	try {
		
		if(checked) {
			
			navigateAct->setChecked(false);
			editAct->setChecked(true);
			zoomAct->setChecked(false);
			panAct->setChecked(false);
			editAddAct->setChecked(false);
			editSubtractAct->setChecked(false);
			editSelectAct->setChecked(true);
			
			OmStateManager::SetToolMode(SELECT_VOXEL_MODE);
			OmEventManager::PostEvent(new OmToolModeEvent(OmToolModeEvent::TOOL_MODE_CHANGE));
			
		}
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}	
}

void MainWindow::updateComboBoxes()
{ 
	try {
		
		DOUT("MainWIndow::updateComboBoxes()");
		selectSegmentationBox->clear();
		//		selectSegmentBox->clear();
		editColorButton->setIcon(QIcon());

		foreach( SegmentIDhelper segH, OmVolume::GetSelectedSegmentIDs() ) {
			QString comboBoxRowStr = segH.segmentationName + "--" + segH.segmentName;
			selectSegmentationBox->addItem( comboBoxRowStr, qVariantFromValue( segH ) );
		}

	} catch (OmException &e) {
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
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}


void MainWindow::changeSelection(int segmentIndex)
{
	DOUT("MainWindow::ChangeSelection");
	try {
		
		QVariant result = selectSegmentationBox->itemData(selectSegmentationBox->currentIndex());
		SegmentIDhelper segH = result.value< SegmentIDhelper >();
		OmId seg_id = segH.segmentationID;
		OmId obj_id = segH.segmentID;
		
		// OmId obj_id = selectSegmentBox->itemData(segmentIndex).toInt();
		// OmId seg_id = selectSegmentationBox->itemData(selectSegmentationBox->currentIndex()).toInt();
		
		if((obj_id != 0) && (seg_id != 0)) {
			
			iSentIt = true;
			OmSegmentEditor::SetEditSelection(seg_id, obj_id);
			iSentIt = false;
			// FIRE EVENT

			OmSegment &current_obj = OmVolume::GetSegmentation(seg_id).GetSegment(obj_id);
			const Vector3<float> &color = current_obj.GetColor();
			
			QPixmap *pixm = new QPixmap(40,30);
			QColor newcolor = qRgb(color.x * 255, color.y * 255, color.z * 255);
			pixm->fill(newcolor);
			
			editColorButton->setIcon(QIcon(*pixm));
			
			selected_color = newcolor;

			// TODO: why is this commented out? (purcaro)
			// iSentIt = true;
			// OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_SELECTION_CHANGE));
		}
		else if (obj_id == 0)
			editColorButton->setIcon(QIcon());
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
}


void MainWindow::closeEvent(QCloseEvent *event)
{
	try {
		
#pragma mark <TODO: Saving position of windows and restoring position of windows> 
		// QMainWindow::saveState() and restoreState()
		
		if(	isProjectOpen )
			if(! checkForSave() )
				event->ignore();
		
		//delete exceptionMessage;
		
		//if(undoView)
			//delete undoView;
		// OmProject::Close();
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
	
}


void MainWindow::SegmentObjectModificationEvent(OmSegmentEvent *event)
{
	try {
		
		DOUT("MainWindow::SegmentObjectModificationEvent");
		// DOUT("segment ID = " << event->mSegmentId);
		
		if(OmStateManager::GetSystemMode() == EDIT_SYSTEM_MODE)
		{
			{
				updateComboBoxes();
			}
		}
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::SegmentEditSelectionChangeEvent(OmSegmentEvent *event)
{
	try {
		
		DOUT("MainWindow::SegmentEditSelectionChangeEvent");
		// DOUT("segment ID = " << event->mSegmentId);
		
		if(! iSentIt) {
			
			if(OmStateManager::GetSystemMode() == EDIT_SYSTEM_MODE)
			{
				DOUT("in sdit system mode");
				OmId segmentation;
				OmId segment;
				if(OmSegmentEditor::GetEditSelection(segmentation, segment)) {
					
					int segmentationIndex = selectSegmentationBox->findData(segmentation);
					// TODO: fix me! (purcaro)
					//					int segmentIndex = selectSegmentBox->findData(segment);
					
					//DOUT("segmentationIndex = " << segmentationIndex << " segmentIndex = " << segmentIndex);
					
// 					if((segmentationIndex != -1) && (segmentIndex != -1)) {
// 						selectSegmentationBox->setCurrentIndex(segmentationIndex);
// 						//						selectSegmentBox->setCurrentIndex(segmentIndex);
// 					}
					//selectSegmentationBox->itemData(selectSegmentationBox->currentIndex()).toInt();
					
				}
				
			}
			
			iSentIt = false;
		}
		else
			DOUT("i sent it!");
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
	
}

void MainWindow::SystemModeChangeEvent(OmSystemModeEvent *event)
{
	DOUT("MainWindow::AlertNotifyEvent");
	
	try {
		
		
		if(OmStateManager::GetSystemMode() == EDIT_SYSTEM_MODE)
		{
			updateComboBoxes();
			
		}
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::AlertNotifyEvent(OmAlertEvent *event)
{
	DOUT("MainWindow::AlertNotifyEvent");
	
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
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::ProgressShow(OmProgressEvent *event)
{
	try {
		
		//	QProgressDialog progress("Copying files...", "Abort Copy", 0, numFiles, this);
		//	progress.setWindowModality(Qt::WindowModal);
		
		/*
		 int GetText() { return mText; }
		 int GetMinimum() { return mMinimum; }
		 int GetMaximum() { return mMaximum; }
		 int GetValue() { return mValue; }
		 */
		DOUT("MainWindow::ProgressShow");
		
		
//		prog_dialog = new QProgressDialog(QString::fromStdString(event->GetText()),
//										  QString("Cancel"),
//										  event->GetMinimum(),
//										  event->GetMaximum(),
//										  this);

		
		prog_dialog.reset();
		prog_dialog.setLabelText(QString::fromStdString(event->GetText()));
		prog_dialog.setCancelButtonText(QString("Cancel"));
		prog_dialog.setMinimum(event->GetMinimum());
		prog_dialog.setMaximum(event->GetMaximum());
		//prog_dialog->setParent(this);
		prog_dialog.setWindowModality(Qt::WindowModal);
		prog_dialog.setValue(event->GetValue());
		OmProgressEvent::SetWasCanceled(false);
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::ProgressHide(OmProgressEvent *event)
{ 
	try {
		
		//prog_dialog->cancel();
		DOUT("MainWindow::ProgressHide");
		//cout << "min: " << event->GetMinimum() << endl;
		//cout << "max: " << event->GetMaximum() << endl;
		//cout << "val: " << event->GetValue() << endl;
		prog_dialog.reset();
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::ProgressRangeEvent(OmProgressEvent *event)
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
		
		DOUT("ProgressRangeEvent");
		//cout << "min: " << event->GetMinimum() << endl;
		//cout << "max: " << event->GetMaximum() << endl;
		//cout << "val: " << event->GetValue() << endl;
		
		prog_dialog.setRange(event->GetMinimum(), event->GetMaximum());
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}
void MainWindow::ProgressValueEvent(OmProgressEvent *event)
{
	try {
		
		DOUT("ProgressValueEvent");
		
		//check for cancel
		OmProgressEvent::SetWasCanceled(prog_dialog.wasCanceled());
		//ignore if progress bar is canceled
		if(prog_dialog.wasCanceled()) return;
		//update value
		prog_dialog.setValue(event->GetValue());
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}
void MainWindow::ProgressIncrementEvent(OmProgressEvent *event)
{
	try {
		
		DOUT("ProgressIncrementEvent");
		//cout << "min: " << event->GetMinimum() << endl;
		//cout << "max: " << event->GetMaximum() << endl;
		//cout << "val: " << event->GetValue() << endl;
		//	int val = prog_bar->value();
		//	prog_bar->setValue(val + 1);
		
		//check for cancel
		OmProgressEvent::SetWasCanceled(prog_dialog.wasCanceled());
		//ignore if progress bar is canceled
		if(prog_dialog.wasCanceled()) return;
		//update value
		prog_dialog.setValue(prog_dialog.value() + 1);
		
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
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
		connect( recentFiles.recentFileActs[i], SIGNAL(triggered()),
			this, SLOT(openRecentFile() ) );
	}
	
	quitAct = new QAction(tr("&Quit"), this);
	quitAct->setShortcut(tr("Ctrl+Q"));
	quitAct->setStatusTip(tr("Quit the application"));
	connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
	
	// Project
	addChannelAct = new QAction(tr("Add &Channel"), this);
	addChannelAct->setShortcut(tr("Ctrl+Shift+C"));
	addChannelAct->setStatusTip(tr("Adds a channe to the current project"));
	connect(addChannelAct, SIGNAL(triggered()), this, SLOT(addChannelToVolume()));
	
	addSegmentationAct = new QAction(tr("Add &Segmentation"), this);
	addSegmentationAct->setShortcut(tr("Ctrl+Shift+S"));
	addSegmentationAct->setStatusTip(tr("Adds a volume to the current project"));
	connect(addSegmentationAct, SIGNAL(triggered()), this, SLOT(addSegmentationToVolume()));
	
	buildAllAct = new QAction(tr("&Build All"), this);
	buildAllAct->setStatusTip(tr("Builds all channels and segmentations"));
	connect(buildAllAct, SIGNAL(triggered()), this, SLOT(buildAll()));
	
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

	createToolbarActions();
	//createToolbarActionsNew();
}


void MainWindow::createToolbarActions()
{
	// Toolbar Actions
	navigateAct = new QAction(QIcon(":/images/pointer.png"),tr("&Navigation Mode"), this);
	// resource location: :/images/cut.png
	navigateAct->setStatusTip(tr("Switches to Navigation Mode"));
	connect(navigateAct, SIGNAL(triggered(bool)), this, SLOT(ChangeModeNavigation(bool)));
	navigateAct->setCheckable(true);
	
	editAct = new QAction(QIcon(":/images/cut.png"),tr("&Edit Mode"), this);
	editAct->setStatusTip(tr("Switches to Edit Mode"));
	connect(editAct, SIGNAL(triggered(bool)), this, SLOT(ChangeModeEdit(bool)));
	editAct->setCheckable(true);
	
	
	zoomAct = new QAction(QIcon(":/images/system-search.png"),tr("&Zoom Mode"), this);
	zoomAct->setStatusTip(tr("Switches to Zoom Mode"));
	connect(zoomAct, SIGNAL(triggered(bool)), this, SLOT(ChangeToolZoom(bool)));
	zoomAct->setCheckable(true);
	
	
	panAct = new QAction(QIcon(":/images/view-pan.png"),tr("&Pan Mode"), this);
	panAct->setStatusTip(tr("Switches to Pan Mode"));
	connect(panAct, SIGNAL(triggered(bool)), this, SLOT(ChangeToolPan(bool)));
	panAct->setCheckable(true);
	
	// list-add.png
	editAddAct = new QAction(QIcon(":/images/list-add.png"),tr("Edit &Addition Mode"), this);
	editAddAct->setStatusTip(tr("Switches to Voxel Addition Mode"));
	connect(editAddAct, SIGNAL(triggered(bool)), this, SLOT(ChangeToolAdd(bool)));
	editAddAct->setCheckable(true);
	
	
	editSubtractAct = new QAction(QIcon(":/images/list-remove.png"),tr("Edit &Subtraction Mode"), this);
	editSubtractAct->setStatusTip(tr("Switches to Voxel Subtraction Mode"));	
	connect(editSubtractAct, SIGNAL(triggered(bool)), this, SLOT(ChangeToolSubtract(bool)));
	editSubtractAct->setCheckable(true);
	
	editSelectAct = new QAction(QIcon(":/images/voxel-select.png"),tr("Edit &Selection Mode"), this);
	editSelectAct->setStatusTip(tr("Switches to Voxel Select Mode"));	
	connect(editSelectAct, SIGNAL(triggered(bool)), this, SLOT(ChangeToolSelect(bool)));
	editSelectAct->setCheckable(true);
	
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
		fileMenu->addAction( recentFiles.recentFileActs[i] );
	}
	fileMenu->addSeparator();
	fileMenu->addAction(quitAct);
	
	editMenu = menuBar()->addMenu(tr("&Edit"));

	projectMenu = menuBar()->addMenu(tr("&Project"));
	projectMenu->addAction(addChannelAct);
	projectMenu->addAction(addSegmentationAct);
	projectMenu->addAction(buildAllAct);
	
	
	toolMenu = menuBar()->addMenu(tr("&Tools"));
	toolMenu->addAction(openOmniInspector);
	toolMenu->addAction(openUndoViewAct);
	
	windowMenu = menuBar()->addMenu(tr("&Window"));
	windowMenu->addAction(open3DAct);
	
	
}


void MainWindow::createToolBars()
{
	
	selectSegmentationBox = new QComboBox();
	selectSegmentationBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	
// 	selectSegmentBox = new QComboBox();
//	selectSegmentBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	
	editColorButton = new QPushButton();
	editColorButton->setMaximumWidth(50);
	
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(saveAct);

	systemToolBar = addToolBar(tr("Mode"));
	systemToolBar->addAction(navigateAct);
	systemToolBar->addAction(editAct);
	
	navigateToolBar = addToolBar(tr("Navigate"));
	navigateToolBar->addAction(zoomAct);
	navigateToolBar->addAction(panAct);
	
	toolToolBar = addToolBar(tr("Tools"));
	// will listen for selection change events and update accordingly
	toolToolBar->addAction(editAddAct);
	toolToolBar->addAction(editSubtractAct);
	toolToolBar->addAction(editSelectAct);
	toolToolBar->addWidget(selectSegmentationBox);
//	toolToolBar->addWidget(selectSegmentBox);
	toolToolBar->addWidget(editColorButton);
	
	editToolBar = addToolBar(tr("Edit"));

}

void MainWindow::createStatusBar()
{
	// statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createDockWindows()
{	 
	
	//	QDockWidget *spacerdock = new QDockWidget(tr("Spacer"), this);
	//	spacerdock->setAllowedAreas(Qt::AllDockWidgetAreas);
	//	
	//	QWidget *spacer = new QWidget;
	//	
	//	spacer->setParent(spacerdock);
	//	spacerdock->setWidget(spacer);
	//	
	//	//	splitDockWidget(mydock, spacerdock, Qt::Vertical);
	//	
	//	addDockWidget(Qt::AllDockWidgetAreas, spacerdock);
	//	viewMenu->addAction(spacerdock->toggleViewAction());
	
	
}


bool MainWindow::checkForSave()
{
	try {
		
		QMessageBox msgBox;
		msgBox.setText("Do you want to save your current project?");
		// msgBox.setInformativeText("Do you want to save the project?");
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Save);
		int ret = msgBox.exec();
		
		
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
				// should never be reached
				break;
		}
		
		
		
		return true;
	}
	
	catch (OmException &e) {
		// DOUT( "Unknown exception!");
		// abort();
		
		spawnErrorDialog(e);
	}
}

void MainWindow::spawnErrorDialog(OmException &e)
{
	DOUT("MainWindow::spawnErrorDialog");
	/*
	 const string& GetName();
	 const string& GetMessage();
	 OmExceptType GetType();
	 */
	
	QString type = QString::fromStdString(e.GetType());
	QString name = QString::fromStdString(e.GetName());
	QString msg = QString::fromStdString(e.GetMessage());
	
	QString errorMessage = type + ": " + name + ". " + msg;
	exceptionMessage->showMessage(errorMessage);
}

void MainWindow::createToolbarActionsNew()
{
	modifyAct = new QAction(tr("&Modify Mode"), this);
	modifyAct->setStatusTip(tr("Switches to Modify Mode"));
	connect(modifyAct, SIGNAL(triggered(bool)), this, SLOT(ChangeModeModify(bool)));
	modifyAct->setCheckable(true);

	toolbarSelectAct    = new QAction(tr("&Select"), this);
	toolbarSelectAct->setStatusTip(tr("Switches to Object Selection Mode"));

	toolbarSelectAct->setCheckable(true);

	toolbarCrosshairAct = new QAction(tr("&Crosshair"), this);
	toolbarCrosshairAct->setStatusTip(tr("Switches on Crosshairs"));

	toolbarCrosshairAct->setCheckable(true);

	toolbarPanAct       = new QAction(tr("&Pan"), this);
	toolbarPanAct->setStatusTip(tr("Switches to Pan Mode"));

	toolbarPanAct->setCheckable(true);

	toolbarZoomAct      = new QAction(tr("&Zoom"), this);
	toolbarZoomAct->setStatusTip(tr("Switches to Zoom Mode"));

	toolbarZoomAct->setCheckable(true);

	toolbarBrushAct     = new QAction(tr("&Brush"), this);
	toolbarBrushAct->setStatusTip(tr("Switches to Voxel Add Mode"));

	toolbarBrushAct->setCheckable(true);

	toolbarEraserAct    = new QAction(tr("&Eraser"), this);
	toolbarEraserAct->setStatusTip(tr("Switches to Voxel Subtraction Mode"));

	toolbarEraserAct->setCheckable(true);

	toolbarFillAct      = new QAction(tr("&Fill"), this);
	toolbarFillAct->setStatusTip(tr("Switches to Fill Mode"));

	toolbarFillAct->setCheckable(true);

	toolbarVoxelizeAct  = new QAction(tr("&Voxelize"), this);
	toolbarVoxelizeAct->setStatusTip(tr("Switches to Voxel Mode"));

	toolbarVoxelizeAct->setCheckable(true);
}

void MainWindow::createToolBarsNew()
{
	
	selectSegmentationBox = new QComboBox();
	selectSegmentationBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	
	editColorButton = new QPushButton();
	editColorButton->setMaximumWidth(50);
	
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(saveAct);

	systemToolBar = addToolBar(tr("Mode"));
	systemToolBar->addAction(modifyAct);
	
	navigateToolBar = addToolBar(tr("Navigate"));
	navigateToolBar->addAction(toolbarSelectAct);
	navigateToolBar->addAction(toolbarCrosshairAct);
	navigateToolBar->addAction(toolbarPanAct);
	navigateToolBar->addAction(toolbarZoomAct);
	
	toolToolBar = addToolBar(tr("Tools"));
	toolToolBar->addAction(toolbarBrushAct);
	toolToolBar->addAction(toolbarEraserAct);
	toolToolBar->addAction(toolbarFillAct);
	toolToolBar->addAction(toolbarJoinAct);
	toolToolBar->addAction(toolbarVoxelizeAct);

	toolToolBar->addWidget(selectSegmentationBox);
	toolToolBar->addWidget(editColorButton);
}

void MainWindow::ChangeModeModify(const bool checked)
{
	try {
		if(checked) {
			//			editSelectAct->setChecked(false);
		}			
		OmStateManager::SetSystemMode( MODIFY_SYSTEM_MODE );
		OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
	} catch (OmException &e) {
		spawnErrorDialog(e);
	}
}

void MainWindow::toolbarSelect(const bool checked)
{
}
void MainWindow::toolbarCrosshair(const bool checked)
{
}
void MainWindow::toolbarPan(const bool checked)
{
}
void MainWindow::toolbarZoom(const bool checked)
{
}
void MainWindow::toolbarBrush(const bool checked)
{
}
void MainWindow::toolbarEraser(const bool checked)
{
}
void MainWindow::toolbarFill(const bool checked)
{
}
void MainWindow::toolbarJoin(const bool checked)
{
}
void MainWindow::toolbarVoxelize(const bool checked)
{
}
/*
void MainWindow::resetTools()
{
	resetViewTools();
	resetModifyTools( false );

	toolbarPanAct->setChecked( true );

	switch( OmSystemMode::GetSystemMode() ){
	case( VIEW_SYSTEM_MODE ):
		break;
	case( MODIFY_SYSTEM_MODE ):
		resetModifyTools( true );
		break;
	default:
	}
}

void MainWindow::resetTool( QAction* tool, const bool enabled )
{
	tool->setChecked(false);
	tool->setEnabled(enabled);
}

void MainWindow::resetViewTools()
{
	resetTool( toolbarSelectAct,    true );
	resetTool( toolbarCrosshairAct, true );
	resetTool( toolbarPanAct,       true );
	resetTool( toolbarZoomAct,      true );
}

void MainWindow::resetModifyTools( const bool enabled )
{
	resetTool( toolbarBrushAct,  enabled );
	resetTool( toolbarEraserAct, enabled );
	resetTool( toolbarFillAct,   enabled );
	resetTool( toolbarJoinAct,   enabled );
	resetTool( toolbarVoxelizeAct, enabled );
}
*/

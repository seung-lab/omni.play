#include <QtGui>

#include "dmainwindow.h"
// REPLACE THIS WITH YOUR CODE
#include "domView2d.h"

#include "system/omStateManager.h"
#include "system/omProject.h"
#include "segment/omSegmentManager.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "common/omSerialization.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs=boost::filesystem;

#include <fstream>


MainWindow::MainWindow()
{
	
	
	loadingDock = new QFrame;
	loadingDock->setFrameStyle(QFrame::Box | QFrame::Raised);
	
	// NO CENTRAL WIDGET for now
	// setCentralWidget(loadingDock);
	
	QGridLayout *dockLayout = new QGridLayout;
	
	
	loadingDock->setLayout(dockLayout);
	

	
	char filename[] = "rabbit.omni";
	
//	if(!bfs::exists(bfs::path(filename))) {
//		OmProject::New("./", filename);
//		
//		string raw1path = "../data/rabbit_small/raw1/tif/";
//		string seg1path = "../data/rabbit_small/seg1/tif/";
//		string seg2path = "../data/rabbit_small/seg2/tif/";
//		string fpattern = "data.%d.tif";
//		
//		string bigrawpath = "../data/0219/raw/tif/";
//		string bigseg1path = "../data/0219/seg/tif/";
//		
//		string marvin_0219_raw = "/Volumes/Marvin/Omni/0219/raw/tif/";
//		string marvin_0219_seg = "/Volumes/Marvin/Omni/0219/seg/tif/";
//		
//		//Vector3i dims(187,321,100);
//		//OmVolume::SetDataDimensions(dims);
//		
//		Vector3f scale(22.6, 22.6, 50);
//		OmVolume::SetScale(scale);
//		
//		
//		OmSegmentation &r_segmentation1 = OmVolume::AddSegmentation();
//		r_segmentation1.SetSourceDirectoryPath(seg1path);
//		r_segmentation1.SetSourceFilePattern(fpattern);
//		r_segmentation1.SetChunkDimension(128);
//		r_segmentation1.BuildVolumeData();
//		r_segmentation1.BuildMeshData();
//		r_segmentation1.ClearCache();
//		
//		OmSegmentation &r_segmentation2 = OmVolume::AddSegmentation();
//		r_segmentation2.SetSourceDirectoryPath(seg2path);
//		r_segmentation2.SetSourceFilePattern(fpattern);
//		r_segmentation2.SetChunkDimension(128);
//		r_segmentation2.BuildVolumeData();
//		r_segmentation2.BuildMeshData();
//		r_segmentation2.ClearCache();
//		
//		OmChannel &r_channel = OmVolume::AddChannel();
//		r_channel.SetSourceDirectoryPath(raw1path);
//		r_channel.SetSourceFilePattern(fpattern);
//		r_channel.SetChunkDimension(128);
//		r_channel.BuildVolumeData();
//		r_channel.ClearCache();
//		
//		
//		//Vector3f scale(751, 751, 200);
//		//Vector3f scale(187, 321, 100);
//		
////		SpaceBbox bbox = OmStateManager::GetViewBbox();
////		bbox.setMax(Vector3f(10,10,10));
////		OmStateManager::SetViewBbox(bbox);
////		OmStateManager::SetViewCenter(Vector3f(5,5,5));
//		
//		cout << "saving project" << endl;
//		//save_volume_manager(*OmVolumeManager::Instance(), filename);
//		//OmProject::SaveAs("./", filename);
//		OmProject::Save();
//	}
	
	
	cout << "load project" << endl;
	OmProject::Load("./", filename);
	

	
//	SpaceBbox bbox = OmStateManager::GetViewBbox();
//	bbox.setMin(Vector3f(0,0,0));
//	bbox.setMax(Vector3f(1,1,1));
//	OmStateManager::SetViewBbox(bbox);
//	OmStateManager::SetViewCenter(Vector3f(0,0,0));
	
	qtView2d_xy = new DOmView2d(XY_VIEW, CHANNEL, 1, 1);
	qtView2d_xz = new DOmView2d(XZ_VIEW, CHANNEL, 1, 1);
	qtView2d_yz = new DOmView2d(YZ_VIEW, CHANNEL, 1, 1);
	
	qtView2d_xy_s = new DOmView2d(XY_VIEW, SEGMENTATION, 1, 1);
	qtView2d_xz_s = new DOmView2d(XZ_VIEW, SEGMENTATION, 1, 1);
	qtView2d_yz_s = new DOmView2d(YZ_VIEW, SEGMENTATION, 1, 1);
	
	
	//qtView3d->setFocus();
	//qtView3d->grabKeyboard();
	
	
	
	//continue with qt setup
	createActions();
	createMenus();
	createToolBars();
	createStatusBar();
	createDockWindows();
	
	setWindowTitle(tr("Dock Widgets"));
	resize(640,480);
}

void MainWindow::open()
{
	
}

void MainWindow::about()
{
	/*
	 QMessageBox::about(this, tr("About Dock Widgets"),
	 tr("The <b>Dock Widgets</b> example demonstrates how to "
	 "use Qt's dock widgets. You can enter your own text, "
	 "click a customer to add a customer name and "
	 "address, and click standard paragraphs to add them."));
	 */
}

void MainWindow::createActions()
{

	
	openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setStatusTip(tr("Open a new channel"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
	
	
	quitAct = new QAction(tr("&Quit"), this);
	quitAct->setShortcut(tr("Ctrl+Q"));
	quitAct->setStatusTip(tr("Quit the application"));
	connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
	
	aboutAct = new QAction(tr("&About"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
	
	aboutQtAct = new QAction(tr("About &Qt"), this);
	aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAct);
	
	editMenu = menuBar()->addMenu(tr("&Edit"));
	
	viewMenu = menuBar()->addMenu(tr("&View"));
	
	menuBar()->addSeparator();
	
	helpMenu = menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
	/*
     fileToolBar = addToolBar(tr("File"));
	 fileToolBar->addAction(openAct);
	 
     editToolBar = addToolBar(tr("Edit"));
	 */
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createDockWindows()
{
	/*
	 qtView2d_xy = new OmView2d(XY_VIEW, CHANNEL, 1);
	 qtView2d_xz = new OmView2d(XZ_VIEW, CHANNEL, 1);
	 qtView2d_yz = new OmView2d(YZ_VIEW, CHANNEL, 1);
	 */
	QDockWidget *dock_xy = new QDockWidget(tr("2D View -- XY"), this);
	dock_xy->setAllowedAreas(Qt::AllDockWidgetAreas);
	
	// REPLACE TETRAHEDRON WITH YOUR CODE

	qtView2d_xy->setParent(dock_xy);
	dock_xy->setWidget(qtView2d_xy);
	
	addDockWidget(Qt::TopDockWidgetArea, dock_xy);
	viewMenu->addAction(dock_xy->toggleViewAction());
	
	
	QDockWidget *dock_xy_s = new QDockWidget(tr("2D View -- XY Seg"), this);
	dock_xy_s->setAllowedAreas(Qt::AllDockWidgetAreas);
	
	// REPLACE TETRAHEDRON WITH YOUR CODE
	
	qtView2d_xy_s->setParent(dock_xy_s);
	dock_xy_s->setWidget(qtView2d_xy_s);
	
	addDockWidget(Qt::TopDockWidgetArea, dock_xy_s);
	viewMenu->addAction(dock_xy_s->toggleViewAction());
	
	
	//QDockWidget *dock_xz = new QDockWidget(tr("2D View -- XZ"), this);
//	dock_xz->setAllowedAreas(Qt::AllDockWidgetAreas);
//	
//	// REPLACE TETRAHEDRON WITH YOUR CODE
//	
//	qtView2d_xz->setParent(dock_xz);
//	dock_xz->setWidget(qtView2d_xz);
//	
//	addDockWidget(Qt::TopDockWidgetArea, dock_xz);
//	viewMenu->addAction(dock_xz->toggleViewAction());
//	
//	
//	QDockWidget *dock_xz_s = new QDockWidget(tr("2D View -- XZ Seg"), this);
//	dock_xz_s->setAllowedAreas(Qt::AllDockWidgetAreas);
//	
//	// REPLACE TETRAHEDRON WITH YOUR CODE
//	
//	qtView2d_xz_s->setParent(dock_xz_s);
//	dock_xz_s->setWidget(qtView2d_xz_s);
//	
//	addDockWidget(Qt::TopDockWidgetArea, dock_xz_s);
//	viewMenu->addAction(dock_xz_s->toggleViewAction());
//	
//	
//	
//	
//	QDockWidget *dock_yz = new QDockWidget(tr("2D View -- YZ"), this);
//	dock_yz->setAllowedAreas(Qt::AllDockWidgetAreas);
//	
//	// REPLACE TETRAHEDRON WITH YOUR CODE
//	
//	qtView2d_yz->setParent(dock_yz);
//	dock_yz->setWidget(qtView2d_yz);
//	
//	addDockWidget(Qt::TopDockWidgetArea, dock_yz);
//	viewMenu->addAction(dock_yz->toggleViewAction());
//	
//	QDockWidget *dock_yz_s = new QDockWidget(tr("2D View -- YZ Seg"), this);
//	dock_yz_s->setAllowedAreas(Qt::AllDockWidgetAreas);
//	
//	// REPLACE TETRAHEDRON WITH YOUR CODE
//	
//	qtView2d_yz_s->setParent(dock_yz_s);
//	dock_yz_s->setWidget(qtView2d_yz_s);
//	
//	addDockWidget(Qt::TopDockWidgetArea, dock_yz_s);
//	viewMenu->addAction(dock_yz_s->toggleViewAction());
	
	
	setDockNestingEnabled(true);
	
}

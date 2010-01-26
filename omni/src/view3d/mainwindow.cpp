#include <QtGui>

#include "mainwindow.h"
// REPLACE THIS WITH YOUR CODE
#include "omView3d.h"

#include "system/omStateManager.h"
#include "system/omProject.h"
#include "segment/omSegmentManager.h"
#include "volume/omSegmentation.h"
#include "volume/omChannel.h"
#include "volume/omVolume.h"
#include "common/omSerialization.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs=boost::filesystem;

#include <fstream>
#include "system/omDebug.h"



/*

void create_test() {
	
	string regex = ".*tif";
//	string seg_path = "/Users/bwarne/MIT/School_Work_08-09/MEng/workspace/matlab/out/";
	string seg_path = "/Users/bwarne/MIT/School_Work_08-09/MEng/workspace/trunk_02_06_09/data/rabbit_small/seg1/tif/";
	
	//user needs to define data dimension (all mipvolumes will use this)
	//Vector3i dims(187,321,100);
	//Vector3i dims(751,751,751);
	//OmVolume::SetDataDimensions(dims);
	
	
	//scale of volume
	Vector3f scale(10, 10, 10);
	OmVolume::SetScale(scale);
	
	//OmVolume::SetChunkDimension(10);
	
	
	OmSegmentation &r_segmentation1 = OmVolume::AddSegmentation();
	r_segmentation1.SetSourceDirectoryPath(seg_path);
	r_segmentation1.SetSourceFilenameRegex(regex);
	
	
	cout << endl;
	cout << "build segmentation volume" << endl;
	r_segmentation1.BuildVolumeData();
	
	
	cout << endl;
	cout << "build segmentation mesh" << endl;
	r_segmentation1.BuildMeshData();
	cout << "done" << endl;
	
		
	cout << "saving project" << endl;
	//OmProject::SaveAs("./", filename);
	OmProject::Save();
	
}




void create_rabbit() {
	
	string regex = ".*tif";
	string fpattern = "data.%d.tif";
	string rabbit_small_raw1 = "../data/rabbit_small/raw1/tif/";
	string rabbit_small_seg1 = "../data/rabbit_small/seg1/tif/";
	string rabbit_small_seg2 = "../data/rabbit_small/seg2/tif/";
	
	string linux_0219_raw = "/Users/bwarne/omni/data/0219/raw/tif/'";
	string linux_0219_seg = "/Users/bwarne/omni/data/0219/seg/tif/";
	
	//user needs to define data dimension (all mipvolumes will use this)
	//Vector3i dims(187,321,100);
	//Vector3i dims(751,751,751);
	//OmVolume::SetDataDimensions(dims);
	
	//scale of volume
	Vector3f scale(22.6, 22.6, 50);
	OmVolume::SetScale(scale);
	
	
	 OmChannel &r_channel = OmVolume::AddChannel();
	 r_channel.SetSourceDirectoryPath(rabbit_small_raw1);
	 r_channel.SetSourceFilenameRegex(regex);
	
	cout << endl;
	cout << "build channel volume" << endl;
	 r_channel.BuildVolumeData();
	 r_channel.ClearCache();
	 
	
	OmSegmentation &r_segmentation1 = OmVolume::AddSegmentation();
	r_segmentation1.SetSourceDirectoryPath(rabbit_small_seg1);
	r_segmentation1.SetSourceFilenameRegex(regex);
	
	
	cout << endl;
	cout << "build segmentation volume" << endl;
	r_segmentation1.BuildVolumeData();
	
	
	cout << endl;
	cout << "build segmentation mesh" << endl;
	r_segmentation1.BuildMeshData();
	cout << "done" << endl;
	

	
	cout << "saving project" << endl;
	//OmProject::SaveAs("./", filename);
	OmProject::Save();
	
}





void create_0219() {
	string fpattern = "data.%d.tif";
	string small_0219_raw = "../../../data/0219_small/raw/tif/";
	string small_0219_seg = "../../../data/0219_small/seg/tif/";
	
	string marvin_0219_raw = "/Volumes/Marvin/omni/0219/raw/tif/";
	string marvin_0219_seg = "/Volumes/Marvin/omni/0219/seg/tif/";
	
	Vector3i dims(751,751,751);
	OmVolume::SetDataDimensions(dims);
	
	//scale of volume
	Vector3f scale(751, 751, 751);
	OmVolume::SetScale(scale);
	
	
	OmSegmentation &r_segmentation = OmVolume::AddSegmentation();
	r_segmentation.SetSourceDirectoryPath(marvin_0219_seg);
	
	cout << endl;
	cout << "build segmentation volume" << endl;
	r_segmentation.BuildVolumeData();
	
	
	cout << endl;
	cout << "build segmentation mesh" << endl;
	r_segmentation.BuildMeshData();
	cout << "done" << endl;
}





void create_raw_0219() {
	string fpattern = "data.%d.tif";
	string small_0219_raw = "../../../data/0219_small/raw/tif/";
	string small_0219_seg = "../../../data/0219_small/seg/tif/";
	
	string marvin_0219_raw = "/Volumes/Marvin/omni/0219/raw/tif/";
	string marvin_0219_seg = "/Volumes/Marvin/omni/0219/seg/tif/";
	
	Vector3i dims(751,751,751);
	OmVolume::SetDataDimensions(dims);
	
	//scale of volume
	Vector3f scale(751, 751, 751);
	OmVolume::SetScale(scale);
	
	
	 OmChannel &r_channel = OmVolume::AddChannel();
	 r_channel.SetSourceDirectoryPath(marvin_0219_raw);


	 cout << endl;
	 cout << "build channel volume" << endl;
	 r_channel.BuildVolumeData();
	 r_channel.ClearCache();
	
	
	OmSegmentation &r_segmentation = OmVolume::AddSegmentation();
	r_segmentation.SetSourceDirectoryPath(marvin_0219_seg);

	
	cout << endl;
	cout << "build segmentation volume" << endl;
	r_segmentation.BuildVolumeData();
	
}















void time_build(string dpath, string fname, string srcdpath) {
	
	int size;
	time_t start_time, end_time;
	
	OmSegmentation &r_segmentation = OmVolume::AddSegmentation();
	r_segmentation.SetSourceDirectoryPath(srcdpath);
	r_segmentation.SetSourceFilenameRegex(".*tif");
	OmVolume::SetDataDimensions(r_segmentation.GetSourceDataVolume().GetDataDimensions());
	cout << r_segmentation.GetSourceDataVolume().GetDataDimensions() << endl;
	
	fprintf(stderr, "--------------------------------------\n");
	cout << "segmentation: " << srcdpath << endl;
	fprintf(stderr, "segmentation: %s\n", srcdpath.c_str());
	cout << endl;
	cout << "build segmentation volume" << endl;
	
	
	time(&start_time);
	r_segmentation.BuildVolumeData();
	time(&end_time);
	fprintf(stderr, "elapsed volume build time: %f\n", difftime(end_time, start_time));
	
	OmProject::Save();
	size = bfs::file_size(bfs::path(dpath+fname));	
	fprintf(stderr, "project size: %d\n", size);
	
	
	
	cout << endl;
	cout << "build segmentation mesh" << endl;
	
	time(&start_time);
	r_segmentation.BuildMeshData();
	time(&end_time);
	fprintf(stderr, "elapsed mesh build time: %f\n", difftime(end_time, start_time));
	
	OmProject::Save();
	size = bfs::file_size(bfs::path(dpath+fname));	
	fprintf(stderr, "project size: %d\n\n\n", size);
}

*/








MainWindow::MainWindow()
{
	
	
	loadingDock = new QFrame;
	loadingDock->setFrameStyle(QFrame::Box | QFrame::Raised);
	
	// NO CENTRAL WIDGET for now
	// setCentralWidget(loadingDock);
	
	QGridLayout *dockLayout = new QGridLayout;
	
	
	loadingDock->setLayout(dockLayout);
	
	
	
	
	string dpath = "/Users/bwarne/MIT/School_Work_08-09/MEng/workspace/trunk_02_06_09/bin/";
	string fname = "proj.omni";
	string srcdpath = "/Users/bwarne/MIT/School_Work_08-09/MEng/data/rabbit_small/seg1/tif/";
	
	if(!bfs::exists(bfs::path(dpath+fname))) {
		OmProject::New(dpath, fname);

		//create_raw_0219();
		//create_rabbit();
		//create_0219();
		//time_build(dpath, fname, srcdpath);
		//create_test();
		
		OmProject::Save();

	} else {
		
		cout << "project already exists" << endl;
		OmProject::Load(dpath, fname);
	}
	

	cout << "done" << endl;

	
	
	
	
	
	
	
	qtView3d = new OmView3d;
	
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
	QDockWidget *dock = new QDockWidget(tr("3D View"), this);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	
	// REPLACE TETRAHEDRON WITH YOUR CODE
	qtView3d->setParent(dock);
	dock->setWidget(qtView3d);

	addDockWidget(Qt::TopDockWidgetArea, dock);
	viewMenu->addAction(dock->toggleViewAction());
	
	
	setDockNestingEnabled(true);
}

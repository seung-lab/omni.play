#include "gui/menubar.h"
#include "gui/mainwindow.h"
#include "project/omProject.h"

MenuBar::MenuBar( MainWindow * mw )
	: QWidget(mw)
{
	mMainWindow = mw;
	createActions();
	createMenus();

	recentFiles.loadRecentlyUsedFilesListFromFS();
}

void MenuBar::createMenus()
{
	fileMenu = mMainWindow->menuBar()->addMenu(tr("&File"));
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

	editMenu = mMainWindow->menuBar()->addMenu(tr("&Edit"));
	editMenu->addAction(editLocalPreferencesAct);
	editMenu->addAction(editPreferencesAct);

	projectMenu = mMainWindow->menuBar()->addMenu(tr("&Project"));
	projectMenu->addAction(addChannelAct);
	projectMenu->addAction(addSegmentationAct);

	toolMenu = mMainWindow->menuBar()->addMenu(tr("&Tools"));
	toolMenu->addAction(openOmniInspector);
	toolMenu->addAction(openUndoViewAct);
	toolMenu->addAction(openCacheMonitorAct);
	toolMenu->addAction(openGroupsTableAct);

	windowMenu = mMainWindow->menuBar()->addMenu(tr("&Window"));
	windowMenu->addAction(open3DAct);
}

void MenuBar::createActions()
{
	// Menubar
	// File
	newAct = new QAction(tr("&New Project..."), mMainWindow);
	newAct->setShortcut(tr("Ctrl+N"));
	newAct->setStatusTip(tr("Begin a new project"));
	connect(newAct, SIGNAL(triggered()), mMainWindow, SLOT(newProject()));

	openAct = new QAction(tr("&Open..."), mMainWindow);
	openAct->setShortcut(tr("Ctrl+O"));
	openAct->setStatusTip(tr("Open an existing project"));
	connect(openAct, SIGNAL(triggered()), mMainWindow, SLOT(openProject()));

	closeAct = new QAction(tr("&Close"), mMainWindow);
	closeAct->setStatusTip(tr("Close current project"));
	connect(closeAct, SIGNAL(triggered()), mMainWindow, SLOT(closeProject()));

	saveAct = new QAction(tr("&Save"), mMainWindow);
	saveAct->setShortcut(tr("Ctrl+S"));
	saveAct->setStatusTip(tr("Saves the current project"));
	connect(saveAct, SIGNAL(triggered()), mMainWindow, SLOT(saveProject()));

	for (int i = 0; i < recentFiles.getMaxNumberOfRecentlyUsedFilesToDisplay(); i++) {
		recentFiles.recentFileActs[i] = new QAction(mMainWindow);
		recentFiles.recentFileActs[i]->setVisible(false);
		connect(recentFiles.recentFileActs[i], SIGNAL(triggered()), mMainWindow, SLOT(openRecentFile()));
	}

	quitAct = new QAction(tr("&Quit"), mMainWindow);
	quitAct->setShortcut(tr("Ctrl+Q"));
	quitAct->setStatusTip(tr("Quit the application"));
	connect(quitAct, SIGNAL(triggered()), mMainWindow, SLOT(close()));

	// Edit
	editLocalPreferencesAct = new QAction(tr("&Local Preferences"), mMainWindow);
	connect(editLocalPreferencesAct, SIGNAL(triggered()), mMainWindow, SLOT(showEditLocalPreferencesDialog()));

	editPreferencesAct = new QAction(tr("&Project Preferences"), mMainWindow);
	connect(editPreferencesAct, SIGNAL(triggered()), mMainWindow, SLOT(showEditPreferencesDialog()));

	// Project
	addChannelAct = new QAction(tr("Add &Channel"), mMainWindow);
	addChannelAct->setShortcut(tr("Ctrl+Shift+C"));
	addChannelAct->setStatusTip(tr("Adds a channe to the current project"));
	connect(addChannelAct, SIGNAL(triggered()), mMainWindow, SLOT(addChannelToVolume()));

	addSegmentationAct = new QAction(tr("Add &Segmentation"), mMainWindow);
	addSegmentationAct->setShortcut(tr("Ctrl+Shift+S"));
	addSegmentationAct->setStatusTip(tr("Adds a volume to the current project"));
	connect(addSegmentationAct, SIGNAL(triggered()), mMainWindow, SLOT(addSegmentationToVolume()));

	// Tools
	openOmniInspector = new QAction(tr("&Inspector"), mMainWindow);
	openOmniInspector->setShortcut(tr("Ctrl+I"));
	openOmniInspector->setStatusTip(tr("Opens the Omni Inspector"));
	connect(openOmniInspector, SIGNAL(triggered()), mMainWindow, SLOT(openInspector()));

	openUndoViewAct = new QAction(tr("&History"), mMainWindow);
	openUndoViewAct->setStatusTip(tr("Opens the Undo History"));
	connect(openUndoViewAct, SIGNAL(triggered()), mMainWindow, SLOT(openUndoView()));

	openCacheMonitorAct = new QAction(tr("&Cache Monitor"), mMainWindow);
	openUndoViewAct->setStatusTip(tr("Opens the Cache Monitor Tool"));
	connect(openCacheMonitorAct, SIGNAL(triggered()), mMainWindow, SLOT(openCacheMonitor()));

	openGroupsTableAct = new QAction(tr("Inspect &Groups"), mMainWindow);
	openGroupsTableAct->setStatusTip(tr("Opens the Groups Table"));
	connect(openGroupsTableAct, SIGNAL(triggered()), mMainWindow, SLOT(openGroupsTable()));


	// Window
	open3DAct = new QAction(tr("Open &3D View"), mMainWindow);
	open3DAct->setShortcut(tr("Ctrl+3"));
	open3DAct->setStatusTip(tr("Opens the 3D view"));
	connect(open3DAct, SIGNAL(triggered()), mMainWindow, SLOT(open3dView()));
}

void MenuBar::updateReadOnlyRelatedWidgets()
{
	bool toBeEnabled = false;

	if ( mMainWindow->isProjectOpen() && !OmProject::IsReadOnly() ){
		toBeEnabled = true;
	}

	saveAct->setEnabled(toBeEnabled);

	addChannelAct->setEnabled( toBeEnabled );
	addSegmentationAct->setEnabled( toBeEnabled );

	if ( mMainWindow->isProjectOpen() ){
		toBeEnabled = true;
	}
}

QMenu * MenuBar::getWindowMenu()
{
	return windowMenu;
}

QAction * MenuBar::getOpenCacheMonitorAct()
{
	return openCacheMonitorAct;
}

void MenuBar::addRecentFile( QString fileName )
{
	recentFiles.addFile( fileName );
}

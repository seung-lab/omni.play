#include "system/omConnect.hpp"
#include "gui/menubar.h"
#include "gui/mainWindow/mainWindow.h"
#include "project/omProject.h"

MenuBar::MenuBar(MainWindow* mainWindow)
    : QWidget(mainWindow), mainWindow_(mainWindow) {
  createActions();
  createMenus();

  recentFiles_.loadRecentlyUsedFilesListFromFS();
}

void MenuBar::createMenus() {
  fileMenu_ = mainWindow_->menuBar()->addMenu(tr("&File"));
  fileMenu_->addAction(newAct_);
  fileMenu_->addAction(openAct_);
  fileMenu_->addSeparator();
  fileMenu_->addAction(closeAct_);
  fileMenu_->addAction(saveAct_);
  fileMenu_->addSeparator();
  for (int i = 0; i < recentFiles_.getMaxNumberOfRecentlyUsedFilesToDisplay();
       i++) {
    fileMenu_->addAction(recentFiles_.recentFileActs[i]);
  }
  fileMenu_->addSeparator();
  fileMenu_->addAction(quitAct_);

  editMenu_ = mainWindow_->menuBar()->addMenu(tr("&Edit"));
  editMenu_->addAction(editLocalPreferencesAct_);
  editMenu_->addAction(editPreferencesAct_);

  projectMenu_ = mainWindow_->menuBar()->addMenu(tr("&Project"));
  projectMenu_->addAction(addChannelAct_);
  projectMenu_->addAction(addSegmentationAct_);
  projectMenu_->addAction(dumpActionLogAct_);

  toolMenu_ = mainWindow_->menuBar()->addMenu(tr("&Tools"));
  toolMenu_->addAction(openOmniInspector_);
  toolMenu_->addAction(openUndoViewAct_);
  toolMenu_->addAction(openCacheMonitorAct_);

  windowMenu_ = mainWindow_->menuBar()->addMenu(tr("&Window"));
  windowMenu_->addAction(open3DAct_);
}

void MenuBar::createActions() {
  // Menubar
  // File
  newAct_ = new QAction(tr("&New Project..."), mainWindow_);
  newAct_->setShortcut(tr("Ctrl+N"));
  newAct_->setStatusTip(tr("Begin a new project"));
  om::connect(newAct_, SIGNAL(triggered()), mainWindow_, SLOT(newProject()));

  openAct_ = new QAction(tr("&Open..."), mainWindow_);
  openAct_->setShortcut(tr("Ctrl+O"));
  openAct_->setStatusTip(tr("Open an existing project"));
  om::connect(openAct_, SIGNAL(triggered()), mainWindow_, SLOT(openProject()));

  closeAct_ = new QAction(tr("&Close"), mainWindow_);
  closeAct_->setStatusTip(tr("Close current project"));
  om::connect(closeAct_, SIGNAL(triggered()), mainWindow_,
              SLOT(closeProject()));

  saveAct_ = new QAction(tr("&Save"), mainWindow_);
  saveAct_->setShortcut(tr("Ctrl+S"));
  saveAct_->setStatusTip(tr("Saves the current project"));
  om::connect(saveAct_, SIGNAL(triggered()), mainWindow_, SLOT(saveProject()));

  for (int i = 0; i < recentFiles_.getMaxNumberOfRecentlyUsedFilesToDisplay();
       i++) {
    recentFiles_.recentFileActs[i] = new QAction(mainWindow_);
    recentFiles_.recentFileActs[i]->setVisible(false);
    om::connect(recentFiles_.recentFileActs[i], SIGNAL(triggered()),
                mainWindow_, SLOT(openRecentFile()));
  }

  quitAct_ = new QAction(tr("&Quit"), mainWindow_);
  quitAct_->setShortcut(tr("Ctrl+Q"));
  quitAct_->setStatusTip(tr("Quit the application"));
  om::connect(quitAct_, SIGNAL(triggered()), mainWindow_, SLOT(close()));

  // Edit
  editLocalPreferencesAct_ = new QAction(tr("&Local Preferences"), mainWindow_);
  om::connect(editLocalPreferencesAct_, SIGNAL(triggered()), mainWindow_,
              SLOT(showEditLocalPreferencesDialog()));

  editPreferencesAct_ = new QAction(tr("&Project Preferences"), mainWindow_);
  om::connect(editPreferencesAct_, SIGNAL(triggered()), mainWindow_,
              SLOT(showEditPreferencesDialog()));

  // Project
  addChannelAct_ = new QAction(tr("Add &Channel"), mainWindow_);
  addChannelAct_->setShortcut(tr("Ctrl+Shift+C"));
  addChannelAct_->setStatusTip(tr("Adds a channe to the current project"));
  om::connect(addChannelAct_, SIGNAL(triggered()), mainWindow_,
              SLOT(addChannelToVolume()));

  addSegmentationAct_ = new QAction(tr("Add &Segmentation"), mainWindow_);
  addSegmentationAct_->setShortcut(tr("Ctrl+Shift+S"));
  addSegmentationAct_->setStatusTip(tr("Adds a volume to the current project"));
  om::connect(addSegmentationAct_, SIGNAL(triggered()), mainWindow_,
              SLOT(addSegmentationToVolume()));

  dumpActionLogAct_ = new QAction("Dump Action Log", mainWindow_);
  om::connect(dumpActionLogAct_, SIGNAL(triggered()), mainWindow_,
              SLOT(dumpActionLog()));

  // Tools
  openOmniInspector_ = new QAction(tr("&Inspector"), mainWindow_);
  openOmniInspector_->setShortcut(tr("Ctrl+I"));
  openOmniInspector_->setStatusTip(tr("Opens the Omni Inspector"));
  om::connect(openOmniInspector_, SIGNAL(triggered()), mainWindow_,
              SLOT(openInspector()));

  openUndoViewAct_ = new QAction(tr("&History"), mainWindow_);
  openUndoViewAct_->setStatusTip(tr("Opens the Undo History"));
  om::connect(openUndoViewAct_, SIGNAL(triggered()), mainWindow_,
              SLOT(openUndoView()));

  openCacheMonitorAct_ = new QAction(tr("&Cache Monitor"), mainWindow_);
  openUndoViewAct_->setStatusTip(tr("Opens the Cache Monitor Tool"));
  om::connect(openCacheMonitorAct_, SIGNAL(triggered()), mainWindow_,
              SLOT(openCacheMonitor()));

  // Window
  open3DAct_ = new QAction(tr("Open &3D View"), mainWindow_);
  open3DAct_->setShortcut(tr("Ctrl+3"));
  open3DAct_->setStatusTip(tr("Opens the 3D view"));
  om::connect(open3DAct_, SIGNAL(triggered()), mainWindow_, SLOT(open3dView()));
}

void MenuBar::UpdateReadOnlyRelatedWidgets() {
  const bool projectOpen = OmProject::IsOpen();
  const bool readWrite = OmProject::IsOpen() && !OmProject::IsReadOnly();

  saveAct_->setEnabled(readWrite);
  closeAct_->setEnabled(projectOpen);

  addChannelAct_->setEnabled(readWrite);
  addSegmentationAct_->setEnabled(readWrite);
}

void MenuBar::AddRecentFile(const QString& fnp) { recentFiles_.addFile(fnp); }

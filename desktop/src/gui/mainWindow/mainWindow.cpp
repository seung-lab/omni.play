#include "actions/io/omActionDumper.h"
#include "actions/omActions.h"
#include "common/exception.h"
#include "gui/cacheMonitorDialog.h"
#include "gui/mainWindow/centralWidget.hpp"
#include "gui/mainWindow/mainWindow.h"
#include "gui/mainWindow/mainWindowEvents.hpp"
#include "gui/menubar.h"
#include "gui/preferences/preferences.h"
#include "gui/recentFileList.h"
#include "gui/sidebars/left/inspectorWidget.h"
#include "gui/taskSelector/taskSelector.h"
#include "gui/toolbars/toolbarManager.h"
#include "gui/toolbars/loginToolbar/loginToolbar.h"
#include "gui/viewGroup/viewGroup.h"
#include "gui/widgets/omNewFileDialog.hpp"
#include "gui/widgets/omTellInfo.hpp"
#include "project/omProject.h"
#include "system/omAppState.hpp"
#include "system/omConnect.hpp"
#include "system/omGlobalKeyPress.hpp"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/omUndoStack.hpp"
#include "viewGroup/omViewGroupState.h"
#include "task/task.h"
#include "task/taskManager.h"

MainWindow::MainWindow()
    : inspector_(nullptr),
      undoView_(nullptr),
      cacheMonitorDialog_(nullptr),
      mMenuBar(new MenuBar(this)),
      toolBarManager_(nullptr) {

  // Toolbars somehow need to be created after the MenuBar and otherwise
  // would not respond to mouse actions before project is loaded...
  loginToolBar_ = new LoginToolBar(this);
  addToolBar(loginToolBar_);
  // without fake toolbar, main window disapperas on Mac
  //  when MainToolBar gets created...
  fakeToolbarForMac_ = addToolBar("Fake Mac Toolbar");

  setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
  setAnimated(false);

  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  setFocusPolicy(Qt::ClickFocus);

  createStatusBar();

  windowTitleClear();
  resize(1000, 1000);

  om::mainwindow::centralWidget* centralWidget =
      new om::mainwindow::centralWidget(this);
  setCentralWidget(centralWidget);

  updateReadOnlyRelatedWidgets();

  OmAppState::SetMainWindow(this);
  OmAppState::SetTaskSelector(new TaskSelector(this));
}

MainWindow::~MainWindow() {}

// Creates a new project
// Prompts the user for a directory in which to create the new project
void MainWindow::newProject() {
  try {

    if (!closeProjectIfOpen(true)) {
      return;
    }

    OmNewFileDialog diag(this);
    const QString fnp = diag.GetNewFileName();
    if (nullptr == fnp) {
      return;
    }

    const QString fileNameAndPath = OmProject::New(fnp);
    updateGuiFromProjectCreateOrOpen(fileNameAndPath);
  }
  catch (om::Exception& e) {
    spawnErrorDialog(e);
  }
}

void MainWindow::showEditPreferencesDialog() {
  if (!OmProject::IsOpen()) {
    return;
  }

  preferences_ = std::make_unique<Preferences>(this);

  preferences_->showProjectPreferences();
  preferences_->show();
  preferences_->raise();
  preferences_->activateWindow();
}

void MainWindow::showEditLocalPreferencesDialog() {
  preferences_ = std::make_unique<Preferences>(this);

  preferences_->showLocalPreferences();
  preferences_->show();
  preferences_->raise();
  preferences_->activateWindow();
}

void MainWindow::addChannelToVolume() {
  try {
    if (!OmProject::IsOpen()) {
      return;
    }

    if (inspector_) {
      inspector_->addChannelToVolume();
    }
  }
  catch (om::Exception& e) {
    spawnErrorDialog(e);
  }
}

void MainWindow::addSegmentationToVolume() {
  try {
    if (!OmProject::IsOpen()) {
      return;
    }

    if (inspector_) {
      inspector_->addSegmentationToVolume();
    }
  }
  catch (om::Exception& e) {
    spawnErrorDialog(e);
  }
}

void MainWindow::dumpActionLog() {
  try {
    if (!OmProject::IsOpen()) {
      return;
    }

    const QString fnp =
        QFileDialog::getSaveFileName(this, "Action Dump File Name");
    if (nullptr == fnp) {
      return;
    }

    OmActionDumper dumper;
    dumper.Dump(fnp);
    OmTellInfo tell("Wrote action log to " + fnp);
  }
  catch (om::Exception& e) {
    spawnErrorDialog(e);
  }
}

/* returns false if New/Open action is cancelled by user */
bool MainWindow::closeProjectIfOpen(bool closeTask) {
  if (!OmProject::IsOpen()) {
    return true;
  }

  if (closeTask && !om::task::TaskManager::AttemptFinishTask()) {
    return false;
  }

  if (!OmProject::IsReadOnly() && closeTask) {
    // closeTask:
    // We skip this user confirmation and saving if we are starting a task -
    // project saving is handled elsewhere.
    // Using closeTask as condition here is a hack. Ideally we should
    // determine whether the project actually changed instead of building
    // special logic for tasks.
    const int ret = checkForSave();
    switch (ret) {
      case QMessageBox::Save:
        OmActions::Save();
        break;
      case QMessageBox::Discard:
        // Don't Save was clicked
        break;
      case QMessageBox::Cancel:
        return false;
      default:
        break;
    }
  }

  inspectorDock_.reset();
  undoViewDock_.reset();
  preferences_.reset();
  globalKeys_.reset();

  // get rid of remaining QDockWidgets (View2d/3d, etc.)
  Q_FOREACH(QDockWidget * dw, this->findChildren<QDockWidget*>()) {
    delete (dw);
  }

  toolBarManager_->UpdateGuiFromProjectClose();

  events_.reset();  // reset before project close

  OmProject::Close();
  windowTitleClear();

  updateReadOnlyRelatedWidgets();

  vgs_.reset();

  return true;
}

void MainWindow::openRecentFile() {
  QAction* action = qobject_cast<QAction*>(sender());

  if (!action) {
    return;
  }

  openProject(action->data().toString().toStdString(), "");
}

bool MainWindow::openProject() {
  try {
    if (!closeProjectIfOpen(true)) {
      return false;
    }
    // Opens an existing project
    // Prompts the user for their project file
    // Internally, this is the file that has Volume serialized

    QString fileNameAndPath = QFileDialog::getOpenFileName(
        this, tr("Open Project"), "", tr("Project File (*.omni)"));

    if (fileNameAndPath == nullptr) return false;

    // open project at fpath
    loadProject(fileNameAndPath.toStdString(), "");
    return true;
  }
  catch (om::Exception& e) {
    spawnErrorDialog(e);
    return false;
  }
}

bool MainWindow::openProject(const std::string& fileNameAndPath,
                             const std::string& username) {
  if (OmProject::IsOpen(fileNameAndPath, username)) {
    return true;
  }
  if (closeProjectIfOpen(false)) {
    return loadProject(fileNameAndPath, username);
  } else {
    return false;
  }
}

bool MainWindow::loadProject(const std::string& fileNameAndPath,
                             const std::string& username) {
  try {
    OmProject::Load(fileNameAndPath, this, username);
    QCoreApplication::processEvents();
    updateGuiFromProjectCreateOrOpen(fileNameAndPath.c_str());
    return true;
  }
  catch (om::Exception& e) {
    spawnErrorDialog(e);
    return false;
  }
}

void MainWindow::closeProject() {
  try {
    closeProjectIfOpen(true);
  }
  catch (om::Exception& e) {
    spawnErrorDialog(e);
  }
}

void MainWindow::saveProject() {
  // Saves the current project
  // Does not prompt the user
  // Serializes Volume to the current working directory
  OmActions::Save();
}

void MainWindow::openInspector() {
  try {

    if (!OmProject::IsOpen()) {
      return;
    }

    if (inspectorDock_) {
      inspectorDock_->show();
      return;
    }

    inspectorDock_ = std::make_unique<QDockWidget>(tr("Inspector"), this);
    inspector_ = new InspectorWidget(inspectorDock_.get(), this, vgs_.get());

    inspector_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    inspectorDock_->setAllowedAreas(Qt::AllDockWidgetAreas);
    inspectorDock_->setFeatures(QDockWidget::NoDockWidgetFeatures);

    inspector_->setParent(inspectorDock_.get());
    inspectorDock_->setWidget(inspector_);

    addDockWidget(Qt::LeftDockWidgetArea, inspectorDock_.get());
    mMenuBar->GetWindowMenu()->addAction(inspectorDock_->toggleViewAction());
  }
  catch (om::Exception& e) {
    spawnErrorDialog(e);
  }
}

void MainWindow::openUndoView() {
  try {
    if (!OmProject::IsOpen()) {
      return;
    }

    if (undoViewDock_) {
      undoViewDock_->show();
      return;
    }

    undoViewDock_ = std::make_unique<QDockWidget>(tr("Undo/History"), this);
    undoView_ = new QUndoView(undoViewDock_.get());

    undoViewDock_->setAllowedAreas(Qt::AllDockWidgetAreas);

    undoView_->setParent(undoViewDock_.get());
    undoViewDock_->setWidget(undoView_);

    addDockWidget(Qt::TopDockWidgetArea, undoViewDock_.get());
    undoViewDock_->setFloating(true);

    mMenuBar->GetWindowMenu()->addAction(undoViewDock_->toggleViewAction());

    undoView_->setStack(OmStateManager::UndoStack().Get());
  }
  catch (om::Exception& e) {
    spawnErrorDialog(e);
  }
}

void MainWindow::open3dView() {
  try {
    if (!OmProject::IsOpen()) {
      return;
    }

    vgs_.GetViewGroup()->AddView3D();
  }
  catch (om::Exception& e) {
    spawnErrorDialog(e);
  }
}

void MainWindow::closeEvent(QCloseEvent* event) {
  try {
    // QMainWindow::saveState() and restoreState()
    if (!closeProjectIfOpen(true)) {
      event->ignore();
    }
  }
  catch (om::Exception& e) {
    spawnErrorDialog(e);
  }
}

int MainWindow::checkForSave() {
  QMessageBox msgBox;
  msgBox.setText("Do you want to save your current project?");
  msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                            QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::Save);

  return msgBox.exec();
}

void MainWindow::spawnErrorDialog(om::Exception& e) {
  const QString errorMessage(e.what());

  OmTellInfo errorBox(errorMessage);

  log_debugs << "Exception thrown: " << qPrintable(errorMessage);
}

void MainWindow::updateReadOnlyRelatedWidgets() {
  mMenuBar->UpdateReadOnlyRelatedWidgets();

  if (toolBarManager_) {
    toolBarManager_->UpdateReadOnlyRelatedWidgets();
  }
}

void MainWindow::windowTitleSet(QString title) {
  QString str = "Omni - " + title;

  if (OmProject::IsReadOnly()) {
    str = "[READ-ONLY] " + str;
  }

  setWindowTitle(str);
}

void MainWindow::windowTitleClear() { setWindowTitle(tr("Omni")); }

void MainWindow::updateGuiFromProjectCreateOrOpen(QString fileName) {
  events_ = std::make_unique<MainWindowEvents>();

  vgs_ = std::make_unique<OmViewGroupState>(this);

  if (!toolBarManager_) {
    toolBarManager_ = new ToolBarManager(this);
  }

  globalKeys_ = std::make_unique<OmGlobalKeyPress>(this);

  mMenuBar->AddRecentFile(fileName);

  toolBarManager_->UpdateGuiFromProjectLoadOrOpen(vgs_.get());
  // Make the login toolbar always the last one
  addToolBar(loginToolBar_);

  windowTitleSet(fileName);
  openInspector();
  updateReadOnlyRelatedWidgets();

  OmStateManager::UndoStack().SetGlobalShortcut(this);
}

void MainWindow::openCacheMonitor() {
  cacheMonitorDialog_ = new CacheMonitorDialog(this);
  cacheMonitorDialog_->setAttribute(Qt::WA_DeleteOnClose, true);

  cacheMonitorDialog_->show();
  mMenuBar->GetOpenCacheMonitorAct()->setChecked(true);
}

void MainWindow::cleanViewsOnVolumeChange(om::common::ObjectType objectType,
                                          om::common::ID objectId) {
  QString unwantedView3DTitle = "3D";

  Q_FOREACH(QDockWidget * dw, this->findChildren<QDockWidget*>()) {
    if (dw->windowTitle().startsWith(unwantedView3DTitle)) {
      delete dw;
    }
  }

  QString unwantedView2DTitle;
  switch (objectType) {
    case om::common::ObjectType::CHANNEL:
      unwantedView2DTitle = "channel" + QString::number(objectId);
      break;
    case om::common::ObjectType::SEGMENTATION:
      unwantedView2DTitle = "segmentation" + QString::number(objectId);
      break;
    default:
      return;
  }

  Q_FOREACH(QDockWidget * dw, this->findChildren<QDockWidget*>()) {

    if (dw->windowTitle().startsWith(unwantedView2DTitle)) {
      delete (dw);
    }
  }
}

void MainWindow::createStatusBar() {
  statusBarLabel = new QLabel(this);
  statusBarLabel->setText("");
  statusBar()->addWidget(statusBarLabel);
}

void MainWindow::updateStatusBar(QString msg) { statusBarLabel->setText(msg); }

void MainWindow::addToolbarRight(QToolBar* b) {
  addToolBar(Qt::RightToolBarArea, b);
}

void MainWindow::addToolbarTop(QToolBar* b) {
  addToolBar(Qt::TopToolBarArea, b);
}

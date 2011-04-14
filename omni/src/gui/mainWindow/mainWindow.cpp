#include "actions/io/omActionDumper.h"
#include "actions/omActions.h"
#include "common/omException.h"
#include "gui/cacheMonitorDialog.h"
#include "gui/groupsTable/groupsTable.h"
#include "gui/mainWindow/inspectorWidget.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/menubar.h"
#include "gui/preferences/preferences.h"
#include "gui/recentFileList.h"
#include "gui/toolbars/toolbarManager.h"
#include "gui/viewGroup/viewGroup.h"
#include "gui/widgets/omNewFileDialog.hpp"
#include "gui/widgets/omTellInfo.hpp"
#include "gui/widgets/omTellInfo.hpp"
#include "project/omProject.h"
#include "system/omAppState.hpp"
#include "system/omConnect.hpp"
#include "system/omGlobalKeyPress.hpp"
#include "system/omPreferenceDefinitions.h"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/omUndoStack.hpp"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

MainWindow::MainWindow()
    : inspector_(NULL)
    , undoView_(NULL)
    , cacheMonitorDialog_(NULL)
    , toolBarManager_(NULL)
    , mMenuBar(new MenuBar(this))
{
    // without fake toolbar, main window disapperas on Mac
    //  when MainToolBar gets created...
    fakeToolbarForMac_ = addToolBar("Fake Mac Toolbar");

    setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);
    setAnimated(false);

    setFocusPolicy(Qt::ClickFocus);

    loadingDock_ = new QFrame();
    loadingDock_->setFrameStyle(QFrame::Box | QFrame::Raised);

    QGridLayout* dockLayout = new QGridLayout(loadingDock_);
    loadingDock_->setLayout(dockLayout);

    createStatusBar();

    windowTitleClear();
    resize(1000, 1000);

    updateReadOnlyRelatedWidgets();

    OmAppState::SetMainWindow(this);
}

MainWindow::~MainWindow()
{}

// Creates a new project
// Prompts the user for a directory in which to create the new project
void MainWindow::newProject()
{
    try {

        if(!closeProjectIfOpen()) {
            return;
        }

        OmNewFileDialog diag(this);
        const QString fnp = diag.GetNewFileName();
        if(NULL == fnp){
            return;
        }

        const QString fileNameAndPath = OmProject::New(fnp);
        updateGuiFromProjectLoadOrOpen(fileNameAndPath);

    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}

void MainWindow::showEditPreferencesDialog()
{
    if(!OmProject::IsOpen()) {
        return;
    }

    preferences_.reset(new Preferences(this));

    preferences_->showProjectPreferences();
    preferences_->show();
    preferences_->raise();
    preferences_->activateWindow();
}

void MainWindow::showEditLocalPreferencesDialog()
{
    preferences_.reset(new Preferences(this));

    preferences_->showLocalPreferences();
    preferences_->show();
    preferences_->raise();
    preferences_->activateWindow();
}

void MainWindow::addChannelToVolume()
{
    try {
        if(!OmProject::IsOpen()) {
            return;
        }

        if(inspector_) {
            inspector_->addChannelToVolume();
        }

    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}

void MainWindow::addSegmentationToVolume()
{
    try {
        if(!OmProject::IsOpen()) {
            return;
        }

        if(inspector_) {
            inspector_->addSegmentationToVolume();
        }

    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}

void MainWindow::dumpActionLog()
{
    try {
        if(!OmProject::IsOpen()) {
            return;
        }

        const QString fnp = QFileDialog::getSaveFileName(this,
                                                         "Action Dump File Name");
        if(NULL == fnp){
            return;
        }

        OmActionDumper dumper;
        dumper.Dump(fnp);
        OmTellInfo("Wrote action log to " + fnp);

    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}

/* returns false if New/Open action is cancelled by user */
bool MainWindow::closeProjectIfOpen()
{
    if(!OmProject::IsOpen()) {
        return true;
    }

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

    inspectorDock_.reset();
    undoViewDock_.reset();
    preferences_.reset();
    groupsTableDock_.reset();
    globalKeys_.reset();

    // get rid of remaining QDockWidgets (View2d/3d, etc.)
    Q_FOREACH(QDockWidget* dw, this->findChildren<QDockWidget* >()){
        delete(dw);
    }

    toolBarManager_->UpdateGuiFromProjectClose();

    OmProject::Close();
    windowTitleClear();

    updateReadOnlyRelatedWidgets();

    vgs_.reset();

    return true;
}

void MainWindow::openRecentFile()
{
    QAction* action = qobject_cast < QAction* >(sender());

    if(!action) {
        return;
    }

    if(!closeProjectIfOpen()) {
        return;
    }

    openProject(action->data().toString());
}

void MainWindow::openProject()
{
    try {
        if(!closeProjectIfOpen()) {
            return;
        }
        // Opens an existing project
        // Prompts the user for their project file
        // Internally, this is the file that has Volume serialized

        QString fileNameAndPath = QFileDialog::getOpenFileName(this,
                                                               tr("Open Project"), "",
                                                               tr("Project File (*.omni)"));

        if(fileNameAndPath == NULL)
            return;

        //open project at fpath
        openProject(fileNameAndPath);

    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}

void MainWindow::openProject(QString fileNameAndPath)
{
    try {
        OmProject::Load(fileNameAndPath);
        updateGuiFromProjectLoadOrOpen(fileNameAndPath);

    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}

void MainWindow::closeProject()
{
    try {
        closeProjectIfOpen();

    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}

void MainWindow::saveProject()
{
    // Saves the current project
    // Does not prompt the user
    // Serializes Volume to the current working directory
    OmActions::Save();
}

void MainWindow::openInspector()
{
    try {

        if(!OmProject::IsOpen()){
            return;
        }

        if(inspectorDock_)
        {
            inspectorDock_->show();
            return;
        }

        inspectorDock_.reset(new QDockWidget(tr("Inspector"), this));
        inspector_ = new InspectorWidget(inspectorDock_.get(), this, vgs_.get());

        inspector_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

        inspectorDock_->setAllowedAreas(Qt::AllDockWidgetAreas);

        inspector_->setParent(inspectorDock_.get());
        inspectorDock_->setWidget(inspector_);

        addDockWidget(Qt::TopDockWidgetArea, inspectorDock_.get());
        mMenuBar->GetWindowMenu()->addAction(inspectorDock_->toggleViewAction());

    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}

void MainWindow::openUndoView()
{
    try {
        if (!OmProject::IsOpen()) {
            return;
        }

        if(undoViewDock_)
        {
            undoViewDock_->show();
            return;
        }

        undoViewDock_.reset(new QDockWidget(tr("Undo/History"), this));
        undoView_ = new QUndoView(undoViewDock_.get());

        undoViewDock_->setAllowedAreas(Qt::AllDockWidgetAreas);

        undoView_->setParent(undoViewDock_.get());
        undoViewDock_->setWidget(undoView_);

        addDockWidget(Qt::TopDockWidgetArea, undoViewDock_.get());
        undoViewDock_->setFloating(true);

        mMenuBar->GetWindowMenu()->addAction(undoViewDock_->toggleViewAction());

        undoView_->setStack(OmStateManager::UndoStack().Get());

    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}

void MainWindow::openGroupsTable()
{
    try {
        if (!OmProject::IsOpen()) {
            return;
        }

        if(groupsTableDock_)
        {
            groupsTableDock_->show();
            return;
        }

        groupsTable_ = new GroupsTable(vgs_.get());

        groupsTableDock_.reset(new QDockWidget(tr("Groups"), this));
        groupsTableDock_->setAllowedAreas(Qt::AllDockWidgetAreas);

        groupsTable_->setParent(groupsTableDock_.get());
        groupsTableDock_->setWidget(groupsTable_);

        addDockWidget(Qt::TopDockWidgetArea, groupsTableDock_.get());
        mMenuBar->GetWindowMenu()->addAction(groupsTableDock_->toggleViewAction());

    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}


void MainWindow::open3dView()
{
    try {
        if(!OmProject::IsOpen()) {
            return;
        }

        vgs_->GetViewGroup()->AddView3D();

    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    try {
        // QMainWindow::saveState() and restoreState()
        if(!closeProjectIfOpen()){
            event->ignore();
        }
    } catch(OmException& e) {
        spawnErrorDialog(e);
    }
}

int MainWindow::checkForSave()
{
    QMessageBox msgBox;
    msgBox.setText("Do you want to save your current project?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);

    return msgBox.exec();
}

void MainWindow::spawnErrorDialog(OmException& e)
{
    const QString errorMessage(e.what());

    OmTellInfo errorBox(errorMessage);

    printf("Exception thrown: %s\n", qPrintable(errorMessage));
}

void MainWindow::updateReadOnlyRelatedWidgets()
{
    mMenuBar->UpdateReadOnlyRelatedWidgets();

    if(toolBarManager_){
        toolBarManager_->UpdateReadOnlyRelatedWidgets();
    }
}

void MainWindow::windowTitleSet(QString title)
{
    QString str = "Omni - " + title;

    if(OmProject::IsReadOnly()){
        str = "[READ-ONLY] " + str;
    }

    setWindowTitle(str);
}

void MainWindow::windowTitleClear(){
    setWindowTitle(tr("Omni"));
}

void MainWindow::updateGuiFromProjectLoadOrOpen(QString fileName)
{
    vgs_.reset(new OmViewGroupState(this));

    if(!toolBarManager_){
        toolBarManager_ = new ToolBarManager(this);
    }

    globalKeys_.reset(new OmGlobalKeyPress(this));

    mMenuBar->AddRecentFile(fileName);

    toolBarManager_->UpdateGuiFromProjectLoadOrOpen(vgs_.get());

    windowTitleSet(fileName);
    openInspector();
    updateReadOnlyRelatedWidgets();

    OmStateManager::UndoStack().SetGlobalShortcut(this);
}

void MainWindow::openCacheMonitor()
{
    cacheMonitorDialog_ = new CacheMonitorDialog(this);
    cacheMonitorDialog_->setAttribute(Qt::WA_DeleteOnClose, true);

    cacheMonitorDialog_->show();
    mMenuBar->GetOpenCacheMonitorAct()->setChecked(true);
}

void MainWindow::cleanViewsOnVolumeChange(ObjectType objectType, OmID objectId)
{
    QString unwantedView3DTitle = "3D";

    Q_FOREACH(QDockWidget* dw, this->findChildren<QDockWidget* >())
    {
        if(dw->windowTitle().startsWith(unwantedView3DTitle)){
            delete dw;
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

    Q_FOREACH(QDockWidget* dw, this->findChildren<QDockWidget* >()){

        if(dw->windowTitle().startsWith(unwantedView2DTitle)){
            delete(dw);
        }
    }
}

void MainWindow::createStatusBar()
{
    statusBarLabel = new QLabel(this);
    statusBarLabel->setText("");
    statusBar()->addWidget(statusBarLabel);
}

void MainWindow::updateStatusBar(QString msg){
    statusBarLabel->setText(msg);
}

void MainWindow::addToolbarRight(QToolBar* b){
    addToolBar(Qt::RightToolBarArea, b);
}

void MainWindow::addToolbarTop(QToolBar* b){
    addToolBar(Qt::TopToolBarArea, b);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    if(toolBarManager_){
        toolBarManager_->WindowResized(QPoint(event->oldSize().width(),
                                              event->oldSize().height()));
    }
}

void MainWindow::moveEvent(QMoveEvent* event)
{
    if(toolBarManager_){
        toolBarManager_->WindowMoved(event->oldPos());
    }
}

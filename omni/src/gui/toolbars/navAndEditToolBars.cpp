#include "gui/mainwindow.h"
#include "gui/toolbars/navAndEditToolBars.h"
#include "project/omProject.h"
#include "system/events/omToolModeEvent.h"
#include "system/events/omViewEvent.h"
#include "system/omEventManager.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "volume/omChannel.h"
#include "volume/omVolume.h"

NavAndEditToolBars::NavAndEditToolBars( MainWindow * mw )
	: QWidget(mw)
{
	mMainWindow = mw;
	createToolbar();
}

////////////////////////////////////////////////////////////
// Toolbars
////////////////////////////////////////////////////////////
void NavAndEditToolBars::createToolbar()
{
	createToolbarActions();
	addToolbars();
	setupFilterToolbar();
	setToolbarDisabled();
}

void NavAndEditToolBars::setupFilterToolbar()
{
	QLabel* label = new QLabel("Alpha Level: ",this); 

	alphaSlider = new QSlider(Qt::Horizontal, this );
	alphaSlider->setObjectName("alphaSlider");
	QSize size = alphaSlider->sizeHint();
	size.setWidth(200);
	alphaSlider->setMaximumSize(size);

	mSegmentationCombo = new QComboBox(this);
        foreach(OmId segmentationID, OmProject::GetValidSegmentationIds()) {
		QString segString = QString("Segmentation %1").arg(segmentationID) ;
		mSegmentationCombo->insertItem(segmentationID, segString);
	
	}
	connect(mSegmentationCombo, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(filterSegmentationChanged(int)), Qt::DirectConnection);
		
	mChannelCombo = new QComboBox(this);
        foreach(OmId channelID, OmProject::GetValidChannelIds()) {
		QString chanString = QString("Channel %1").arg(channelID) ;
		mChannelCombo->insertItem(channelID, chanString);
	
	}
	connect(mChannelCombo, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(filterChannelChanged(int)), Qt::DirectConnection);
		

	OmId channelID = 1;
	OmId filterID = 1;

	if( OmProject::IsChannelValid( channelID ) ){
		OmChannel& channel = OmProject::GetChannel( channelID );
		if( channel.IsFilterValid( filterID ) ){
			OmFilter2d & filter = OmProject::GetChannel(channelID).GetFilter(filterID);
			alphaSlider->setValue(filter.GetAlpha() * 100);
			OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
 		}
	}

	connect(alphaSlider, SIGNAL(valueChanged(int)), 
		this, SLOT(setFilAlpha(int)), Qt::DirectConnection);
	
	filterToolBar = mMainWindow->addToolBar("Filter");
	filterToolBar->addWidget(label);
	filterToolBar->addWidget( alphaSlider );
	filterToolBar->addWidget(mSegmentationCombo);
	filterToolBar->addWidget(mChannelCombo);
}

void NavAndEditToolBars::updateSilder()
{
	OmId channelID = 1;
	OmId filterID = 1;

	if( OmProject::IsChannelValid( channelID ) ){
		OmChannel& channel = OmProject::GetChannel( channelID );
		if( channel.IsFilterValid( filterID ) ){
			OmFilter2d & filter = OmProject::GetChannel(channelID).GetFilter(filterID);
			alphaSlider->setValue(filter.GetAlpha() * 100);
			OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
 		}
	}
}

void NavAndEditToolBars::setFilAlpha(int alpha)
{
	OmId channelID = 1;
	OmId filterID = 1;

	if( OmProject::IsChannelValid( channelID ) ){
		OmChannel& channel = OmProject::GetChannel( channelID );
		if( channel.IsFilterValid( filterID ) ){
			channel.GetFilter( filterID ).SetAlpha((double)alpha / 100.00);
			OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
 		}
	}
}

void NavAndEditToolBars::setFilterToolbarEnabled( bool setEnabled )
{
	alphaSlider->setEnabled( setEnabled );
}

void NavAndEditToolBars::setToolbarDisabled()
{
	saveToolbarAct->setEnabled(false);
	modifyAct->setEnabled(false);
	toolbarSelectAct->setEnabled(false);
	toolbarCrosshairAct->setEnabled(false);
	toolbarPanAct->setEnabled(false);
	toolbarZoomAct->setEnabled(false);
	toolbarVoxelizeAct->setEnabled(false);
	toolbarBrushAct->setEnabled(false);
	toolbarEraserAct->setEnabled(false);
	toolbarFillAct->setEnabled(false);
	toolbarView2D3DopenAct->setEnabled(false);

	setFilterToolbarEnabled( false );
}

void NavAndEditToolBars::createToolbarActions()
{
	saveToolbarAct = new QAction(tr("&Save"), mMainWindow);
	saveToolbarAct->setShortcut(tr("Ctrl+S"));
	saveToolbarAct->setStatusTip(tr("Saves the current project"));
	connect(saveToolbarAct, SIGNAL(triggered()), mMainWindow, SLOT(saveProject()));

	modifyAct = new QAction(tr("&Modify Mode"), mMainWindow);
	modifyAct->setStatusTip(tr("Switches to Modify Mode"));
	connect(modifyAct, SIGNAL(triggered(bool)), this, SLOT(ChangeModeModify(bool)));
	modifyAct->setCheckable(true);

	toolbarSelectAct = new QAction(tr("&Select"), mMainWindow);
	toolbarSelectAct->setStatusTip(tr("Switches to Object Selection Mode"));
	connect(toolbarSelectAct, SIGNAL(triggered(bool)), this, SLOT(toolbarSelect(bool)));
	toolbarSelectAct->setCheckable(true);

	toolbarCrosshairAct = new QAction(tr("&Crosshair"), mMainWindow);
	toolbarCrosshairAct->setStatusTip(tr("Switches on Crosshairs"));
	connect(toolbarCrosshairAct, SIGNAL(triggered(bool)), this, SLOT(toolbarCrosshair(bool)));
	toolbarCrosshairAct->setCheckable(true);

	toolbarPanAct = new QAction(tr("&Pan"), mMainWindow);
	toolbarPanAct->setStatusTip(tr("Switches to Pan Mode"));
	connect(toolbarPanAct, SIGNAL(triggered(bool)), this, SLOT(toolbarPan(bool)));
	toolbarPanAct->setCheckable(true);

	toolbarZoomAct = new QAction(tr("&Zoom"), mMainWindow);
	toolbarZoomAct->setStatusTip(tr("Switches to Zoom Mode"));
	connect(toolbarZoomAct, SIGNAL(triggered(bool)), this, SLOT(toolbarZoom(bool)));
	toolbarZoomAct->setCheckable(true);

	toolbarVoxelizeAct = new QAction(tr("&Voxelizer"), mMainWindow);
	toolbarVoxelizeAct->setStatusTip(tr("Switches to Voxel Mode"));
	connect(toolbarVoxelizeAct, SIGNAL(triggered(bool)), this, SLOT(toolbarVoxelize(bool)));
	toolbarVoxelizeAct->setCheckable(true);

	toolbarBrushAct = new QAction(tr("&Brush"), mMainWindow);
	toolbarBrushAct->setStatusTip(tr("Switches to Voxel Add Mode"));
	connect(toolbarBrushAct, SIGNAL(triggered(bool)), this, SLOT(toolbarBrush(bool)));
	toolbarBrushAct->setCheckable(true);

	toolbarEraserAct = new QAction(tr("&Eraser"), mMainWindow);
	toolbarEraserAct->setStatusTip(tr("Switches to Voxel Subtraction Mode"));
	connect(toolbarEraserAct, SIGNAL(triggered(bool)), this, SLOT(toolbarEraser(bool)));
	toolbarEraserAct->setCheckable(true);

	toolbarFillAct = new QAction(tr("&Fill"), mMainWindow);
	toolbarFillAct->setStatusTip(tr("Switches to Fill Mode"));
	connect(toolbarFillAct, SIGNAL(triggered(bool)), this, SLOT(toolbarFill(bool)));
	toolbarFillAct->setCheckable(true);

	toolbarView2D3DopenAct = new QAction(tr("&Open 2D and 3D Views"), mMainWindow);
	toolbarView2D3DopenAct->setStatusTip(tr("Open 2D and 3D Views"));
	connect(toolbarView2D3DopenAct, SIGNAL(triggered(bool)), mMainWindow, SLOT(open2Dand3dViews()));
	toolbarView2D3DopenAct->setCheckable(false);
	
}

void NavAndEditToolBars::addToolbars()
{
	fileToolBar = mMainWindow->addToolBar(tr("File"));
	fileToolBar->addAction(saveToolbarAct);

	systemToolBar = mMainWindow->addToolBar(tr("Mode"));
	systemToolBar->addAction(modifyAct);

	navigateToolBar = mMainWindow->addToolBar(tr("Navigate"));
	navigateToolBar->addAction(toolbarSelectAct);
	navigateToolBar->addAction(toolbarCrosshairAct);
	navigateToolBar->addAction(toolbarPanAct);
	navigateToolBar->addAction(toolbarZoomAct);
	navigateToolBar->addAction(toolbarVoxelizeAct);

	toolToolBar = mMainWindow->addToolBar(tr("Tools"));
	toolToolBar->addAction(toolbarBrushAct);
	toolToolBar->addAction(toolbarEraserAct);
	toolToolBar->addAction(toolbarFillAct);

	viewToolBar = mMainWindow->addToolBar(tr("Views"));
	viewToolBar->addAction(toolbarView2D3DopenAct);
}

void NavAndEditToolBars::setupToolbarInitially()
{
	resetTools(NAVIGATION_SYSTEM_MODE);
	setFilterToolbarEnabled( true );

	OmStateManager::SetSystemMode(NAVIGATION_SYSTEM_MODE);
	OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
}

void NavAndEditToolBars::ChangeModeModify(const bool checked)
{
	try {
		if (checked) {
			OmStateManager::SetSystemMode(EDIT_SYSTEM_MODE);
			resetTools(EDIT_SYSTEM_MODE);
		} else {
			if( OmProjectData::IsReadOnly() ){
				return;
			}

			OmStateManager::SetSystemMode(NAVIGATION_SYSTEM_MODE);
			resetTools(NAVIGATION_SYSTEM_MODE);
		}

		OmEventManager::PostEvent(new OmSystemModeEvent(OmSystemModeEvent::SYSTEM_MODE_CHANGE));
	} catch(OmException & e) {
		mMainWindow->spawnErrorDialog(e);
	}
}

void NavAndEditToolBars::toolbarToolChange(const bool checked, QAction * tool, const OmToolMode mode)
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
void NavAndEditToolBars::toolbarSelect(const bool checked)
{
	toolbarToolChange(checked, toolbarSelectAct, SELECT_MODE); 
}

void NavAndEditToolBars::toolbarCrosshair(const bool checked)
{
	toolbarToolChange(checked, toolbarCrosshairAct, CROSSHAIR_MODE);
}

void NavAndEditToolBars::toolbarPan(const bool checked)
{
	toolbarToolChange(checked, toolbarPanAct, PAN_MODE);
}

void NavAndEditToolBars::toolbarZoom(const bool checked)
{
	toolbarToolChange(checked, toolbarZoomAct, ZOOM_MODE);
}

void NavAndEditToolBars::toolbarVoxelize(const bool checked)
{
	toolbarToolChange(checked, toolbarVoxelizeAct, VOXELIZE_MODE);
}

// modify tools
void NavAndEditToolBars::toolbarBrush(const bool checked)
{
	toolbarToolChange(checked, toolbarBrushAct, ADD_VOXEL_MODE);
}

void NavAndEditToolBars::toolbarEraser(const bool checked)
{
	toolbarToolChange(checked, toolbarEraserAct, SUBTRACT_VOXEL_MODE);
}

void NavAndEditToolBars::toolbarFill(const bool checked)
{
	toolbarToolChange(checked, toolbarFillAct, SELECT_VOXEL_MODE);
}

void NavAndEditToolBars::resetTools(const OmSystemMode sys_mode)
{
	switch (sys_mode) {
	case (NAVIGATION_SYSTEM_MODE):
		resetViewTools();
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

void NavAndEditToolBars::resetTool(QAction * tool, const bool enabled)
{
	tool->setChecked(false);
	tool->setEnabled(enabled);
}

void NavAndEditToolBars::resetViewTools()
{
	resetTool(toolbarSelectAct, true);
	resetTool(toolbarCrosshairAct, true);
	resetTool(toolbarPanAct, true);
	resetTool(toolbarZoomAct, true);
	resetTool(toolbarVoxelizeAct, true);
}

void NavAndEditToolBars::resetModifyTools(const bool enabled)
{
	resetTool(toolbarBrushAct, enabled);
	resetTool(toolbarEraserAct, enabled);
	resetTool(toolbarFillAct, enabled);
}

void NavAndEditToolBars::SystemModeChangeEvent()
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

void NavAndEditToolBars::updateReadOnlyRelatedWidgets()
{
	bool toBeEnabled = false;

	if ( mMainWindow->isProjectOpen() && !OmProjectData::IsReadOnly() ){
		toBeEnabled = true;
	}

	saveToolbarAct->setEnabled(toBeEnabled);
	modifyAct->setEnabled(toBeEnabled);

	if ( mMainWindow->isProjectOpen() ){
		toBeEnabled = true;
	}
	
	toolbarView2D3DopenAct->setEnabled( toBeEnabled );
}

void NavAndEditToolBars::updateGuiFromProjectLoadOrOpen(OmViewGroupState *)
{
	updateSilder();
}

void NavAndEditToolBars::filterSegmentationChanged(int)
{
	// Do What Thou Wilst . . . 
}

void NavAndEditToolBars::filterChannelChanged(int chanId)
{
	OmId filterID = 1;

	if( OmProject::IsChannelValid( chanId ) ){
		OmChannel& channel = OmProject::GetChannel( chanId );
		if( channel.IsFilterValid( filterID ) ){
			OmFilter2d & filter = OmProject::GetChannel(chanId).GetFilter(filterID);
			alphaSlider->setValue(filter.GetAlpha() * 100);
			OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
 		}
	}
}

#include "gui/mainwindow.h"
#include "gui/toolbars/dendToolbar.h"
#include "gui/toolbars/navAndEditToolBars.h"
#include "system/events/omToolModeEvent.h"
#include "system/omProjectData.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"
#include "toolbarManager.h"
#include "volume/omVolume.h"

ToolBarManager::ToolBarManager( MainWindow * mw )
	: QWidget(mw)
	, mMainWindow(mw)
	, navAndEditToolBars( new NavAndEditToolBars( mw ) )
	, dendToolBar( new DendToolBar( mw ) )
{
	OmStateManager::setDendToolBar( dendToolBar );
}

void ToolBarManager::setupToolbarInitially()
{
	navAndEditToolBars->setupToolbarInitially();
	dendToolBar->setupToolbarInitially();
}

void ToolBarManager::updateReadOnlyRelatedWidgets()
{
	navAndEditToolBars->updateReadOnlyRelatedWidgets();
	dendToolBar->updateReadOnlyRelatedWidgets();
}

void ToolBarManager::updateGuiFromProjectLoadOrOpen(OmViewGroupState * vgs)
{
	navAndEditToolBars->updateGuiFromProjectLoadOrOpen(vgs);
	dendToolBar->updateGuiFromProjectLoadOrOpen(vgs);
	vgs->SetToolBarManager(this);
}

void ToolBarManager::SystemModeChangeEvent()
{
	navAndEditToolBars->SystemModeChangeEvent();
	dendToolBar->SystemModeChangeEvent();
}

void ToolBarManager::SetSplittingOff()
{
	dendToolBar->SetSplittingOff();
}


#include "gui/toolbars/navAndEditToolBars.h"
#include "gui/mainwindow.h"
#include "volume/omVolume.h"
#include "system/omProjectData.h"
#include "system/events/omToolModeEvent.h"
#include "gui/toolbars/dendToolbar.h"

ToolBarManager::ToolBarManager( MainWindow * mw )
	: QWidget(mw)
{
	mMainWindow = mw;
	navAndEditToolBars = new NavAndEditToolBars( mMainWindow );
	dendToolBar = new DendToolBar( mMainWindow );
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

void ToolBarManager::updateGuiFromPorjectLoadOrOpen(OmViewGroupState * vgs)
{
	navAndEditToolBars->updateGuiFromPorjectLoadOrOpen(vgs);
	dendToolBar->updateGuiFromProjectLoadOrOpen(vgs);
}

void ToolBarManager::SystemModeChangeEvent()
{
	navAndEditToolBars->SystemModeChangeEvent();
	dendToolBar->SystemModeChangeEvent();
}

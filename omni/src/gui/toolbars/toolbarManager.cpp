#include "gui/mainwindow.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/mainToolbar/mainToolbar.h"
#include "gui/toolbars/toolbarManager.h"
#include "system/omStateManager.h"
#include "system/viewGroup/omViewGroupState.h"

ToolBarManager::ToolBarManager( MainWindow * mw )
	: QWidget(mw)
	, mMainWindow(mw)
	, mainToolbar(new MainToolbar(mw))
	, dendToolBar(new DendToolBar(mw))
{
	OmStateManager::setDendToolBar( dendToolBar );
}

void ToolBarManager::setupToolbarInitially()
{
	dendToolBar->setupToolbarInitially();
}

void ToolBarManager::updateReadOnlyRelatedWidgets()
{
	dendToolBar->updateReadOnlyRelatedWidgets();
}

void ToolBarManager::updateGuiFromProjectLoadOrOpen(OmViewGroupState * vgs)
{
	mainToolbar->updateToolbar();
	dendToolBar->updateGuiFromProjectLoadOrOpen(vgs);
	vgs->SetToolBarManager(this);
}

void ToolBarManager::SetSplittingOff()
{
	dendToolBar->SetSplittingOff();
}


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
	, dendToolBar(NULL)
{
	OmStateManager::setDendToolBar( dendToolBar );
}

void ToolBarManager::setupToolbarInitially()
{
}

void ToolBarManager::updateReadOnlyRelatedWidgets()
{
}

void ToolBarManager::updateGuiFromProjectLoadOrOpen(OmViewGroupState* vgs)
{
	mainToolbar->updateToolbar();

	if(dendToolBar){
		mMainWindow->removeToolBar(dendToolBar);
		delete dendToolBar;
	}
	dendToolBar = new DendToolBar(mMainWindow, vgs);

	vgs->SetToolBarManager(this);
}

void ToolBarManager::SetSplittingOff()
{
	if(dendToolBar){
		dendToolBar->SetSplittingOff();
	}
}

void ToolBarManager::setTool(const OmToolMode tool)
{
	mainToolbar->setTool(tool);
}

void ToolBarManager::windowResized(QPoint oldPos)
{
	//	printf("toolbarManager:: resizing to %d, %d\n", oldPos.x(), oldPos.y());
	if(dendToolBar){
		dendToolBar->updateToolBarsPos(oldPos);
	}
}

void ToolBarManager::windowMoved(QPoint oldPos)
{
	//	printf("toolbarManager:: moving to %d, %d\n", oldPos.x(), oldPos.y());
	if(dendToolBar){
		dendToolBar->updateToolBarsPos(oldPos);
	}
}

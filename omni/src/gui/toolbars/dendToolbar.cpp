#include "gui/toolbars/dendToolbar.h"
#include "gui/mainwindow.h"
#include "system/omProjectData.h"

DendToolBar::DendToolBar( MainWindow * mw )
	: QWidget(mw)
{
	mMainWindow = mw;
	createToolbar();
}

////////////////////////////////////////////////////////////
// Toolbars
////////////////////////////////////////////////////////////
void DendToolBar::createToolbar()
{
	createToolbarActions();
	addToolbars();
	//	setToolbarDisabled();
}

void DendToolBar::setToolbarDisabled()
{
	toolbarSplitAct->setEnabled(false);
}

void DendToolBar::createToolbarActions()
{
	toolbarSplitAct = new QAction(tr("Split"), mMainWindow);
	toolbarSplitAct->setStatusTip(tr("Split object mode"));
	connect(toolbarSplitAct, SIGNAL(triggered()), 
		this, SLOT(split()));
	toolbarSplitAct->setCheckable(true);
}

void DendToolBar::addToolbars()
{
	dendToolBar = new QToolBar( "Dend", this );
	dendToolBar->setFloatable( true );
	mMainWindow->addToolBar( Qt::RightToolBarArea, dendToolBar );
	dendToolBar->addAction(toolbarSplitAct);
}

void DendToolBar::setupToolbarInitially()
{
}

void DendToolBar::ChangeModeModify(const bool )
{

}

void DendToolBar::toolbarSplit(const bool )
{

}

void DendToolBar::resetTool(QAction * tool, const bool enabled)
{
	tool->setChecked(false);
	tool->setEnabled(enabled);
}

void DendToolBar::SystemModeChangeEvent()
{
}

void DendToolBar::updateReadOnlyRelatedWidgets()
{
	bool toBeEnabled = false;

	if ( mMainWindow->isProjectOpen() && !OmProjectData::IsReadOnly() ){
		toBeEnabled = true;
	}

	toolbarSplitAct->setEnabled(toBeEnabled);
}

void DendToolBar::split()
{
	if( toolbarSplitAct->isChecked() ) {
		OmStateManager::SetSystemMode(DEND_MODE);	
		OmStateManager::SetDendToolMode(SPLIT);

	} else {
		OmStateManager::SetSystemModePrev();
	}
}

void DendToolBar::updateGuiFromPorjectLoadOrOpen()
{
}

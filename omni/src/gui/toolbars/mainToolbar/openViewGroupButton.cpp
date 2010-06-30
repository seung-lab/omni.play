#include "gui/mainwindow.h"
#include "gui/toolbars/mainToolbar/openViewGroupButton.h"
#include "project/omProject.h"
#include "system/viewGroup/omViewGroupState.h"

OpenViewGroupButton::OpenViewGroupButton(MainWindow * mw)
	: OmButton<MainWindow>( mw, 
				"Display Views", 
				"Open the 2D and 3D views", 
				false)
	, mMainWindow(mw)
{
	setFlat(true);
}

void OpenViewGroupButton::doAction()
{
	const OmId channelID = 1;
	const OmId segmentationID = 1;

	mMainWindow->getViewGroupState()->addAllViews( channelID, segmentationID );
}

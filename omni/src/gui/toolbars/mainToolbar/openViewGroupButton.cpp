#include "gui/mainwindow.h"
#include "gui/toolbars/mainToolbar/openViewGroupButton.h"
#include "project/omProject.h"
#include "viewGroup/omViewGroupState.h"

OpenViewGroupButton::OpenViewGroupButton(MainWindow * mw)
	: OmButton<MainWindow>( mw, 
				"Display Views", 
				"Open the 2D and 3D views", 
				false)
{
	setFlat(true);
	setIconAndText(":/toolbars/mainToolbar/icons/1278009384_kllckety.png");
}

void OpenViewGroupButton::doAction()
{
	const OmID channelID = 1;
	const OmID segmentationID = 1;

	mParent->getViewGroupState()->addAllViews( channelID, segmentationID );
}

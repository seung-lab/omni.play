#include "gui/mainwindow.h"
#include "gui/toolbars/mainToolbar/openViewGroupButton.h"
#include "project/omProject.h"
#include "system/viewGroup/omViewGroupState.h"

OpenViewGroupButton::OpenViewGroupButton(MainWindow * mw)
	: OmButton<MainWindow>( mw, 
				"Display Views", 
				"Open the 2D and 3D views", 
				false)
{
	setFlat(true);
        setIcon(QIcon(":/toolbars/mainToolbar/icons/1278009384_kllckety.png"));
        setText("");
}

void OpenViewGroupButton::doAction()
{
	const OmId channelID = 1;
	const OmId segmentationID = 1;

	mParent->getViewGroupState()->addAllViews( channelID, segmentationID );
}

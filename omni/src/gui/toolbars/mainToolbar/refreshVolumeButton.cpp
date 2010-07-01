#include "gui/toolbars/mainToolbar/refreshVolumeButton.h"
#include "project/omProject.h"

VolumeRefreshButton::VolumeRefreshButton(MainWindow * parent)
	: OmButton<MainWindow>( parent, 
				"Rebuild", 
				"Rebuild image and mesh data", 
				false)
{
	setFlat(true);
}

void VolumeRefreshButton::doAction()
{
	printf("VolumeRefreshButton::doAction(): fixme!\n");
	//OmMipVolume::Flush();
}

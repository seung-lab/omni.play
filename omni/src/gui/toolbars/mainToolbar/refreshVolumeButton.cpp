#include "gui/mainwindow.h"
#include "gui/toolbars/mainToolbar/refreshVolumeButton.h"
#include "project/omProject.h"
#include "volume/omSegmentation.h"

VolumeRefreshButton::VolumeRefreshButton(MainWindow * parent)
	: OmButton<MainWindow>( parent,
				"Rebuild",
				"Rebuild image and mesh data",
				false)
{
	setFlat(true);
	setIconAndText(":/toolbars/mainToolbar/icons/1278008555_agt_softwareD.png");
}

void VolumeRefreshButton::doAction()
{
	const OmIDsSet & set = OmProject::GetValidSegmentationIds();
	OmIDsSet::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){
		printf("fixme!!!!!!!!!!!!!!\n");
		//OmProject::GetSegmentation(*iter).Flush();
	}
}

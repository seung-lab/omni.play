#include "project/omSegmentationManager.h"
#include "gui/mainwindow.h"
#include "gui/toolbars/mainToolbar/refreshVolumeButton.h"
#include "project/omProject.h"
#include "project/omProjectVolumes.h"
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
	const OmIDsSet & set = OmProject::Volumes().Segmentations().GetValidSegmentationIds();
	OmIDsSet::const_iterator iter;
	for( iter = set.begin(); iter != set.end(); ++iter ){
		printf("fixme!!!!!!!!!!!!!!\n");
		//OmProject::Volumes().Segmentations().GetSegmentation(*iter).Flush();
	}
}

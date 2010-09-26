#ifndef SEG_INSPECTOR_BUILD_BUTTON_H
#define SEG_INSPECTOR_BUILD_BUTTON_H

#include "gui/widgets/omButton.h"
#include "gui/inspectors/segmentation/segInspector.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentCache.h"
#include "system/omBuildSegmentation.h"

class BuildButton : public OmButton<SegInspector> {
public:
	BuildButton(SegInspector * d)
		: OmButton<SegInspector>( d,
								  "Build",
								  "Build",
								  false)
	{
	}

private:
	void doAction()
	{
		boost::shared_ptr<SegmentationDataWrapper> sdw =
			mParent->getSegmentationDataWrapper();
		OmSegmentation & current_seg = sdw->getSegmentation();

		OmBuildSegmentation * bs = new OmBuildSegmentation(&current_seg);
		bs->setFileNamesAndPaths( mParent->getFileInfoList() );

		const QString whatOrHowToBuild = mParent->buildComboBox->currentText();
		if ("Data" == whatOrHowToBuild ){
			bs->build_seg_image();
			mParent->rebuildSegmentLists(sdw->getID(), 0);

		} else if ( "Mesh" == whatOrHowToBuild ){
			bs->build_seg_mesh();

		} else if ("Data & Mesh" == whatOrHowToBuild){
			bs->buildAndMeshSegmentation();
			mParent->rebuildSegmentLists(sdw->getID(), 0);

		} else if ("Load Dendrogram" == whatOrHowToBuild){
			bs->loadDendrogram();
			mParent->rebuildSegmentLists(sdw->getID(), 0);

		} else if( "Blank Volume" == whatOrHowToBuild ){
			bs->buildBlankVolume();
		}
	}
};

#endif

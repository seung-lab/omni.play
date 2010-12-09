#ifndef SEG_INSPECTOR_BUILD_BUTTON_H
#define SEG_INSPECTOR_BUILD_BUTTON_H

#include "gui/widgets/omButton.hpp"
#include "gui/inspectors/segmentation/segInspector.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentCache.h"
#include "volume/build/omBuildSegmentation.hpp"

class BuildButton : public OmButton<SegInspector> {
public:
	BuildButton(SegInspector * d)
		: OmButton<SegInspector>( d,
								  "Build",
								  "Build",
								  false)
	{}

private:
	void doAction()
	{
		const SegmentationDataWrapper& sdw = mParent->GetSegmentationDataWrapper();

		OmBuildSegmentation* bs = new OmBuildSegmentation(sdw);

		bs->setFileNamesAndPaths( mParent->getFileInfoList() );

		const QString whatOrHowToBuild = mParent->buildComboBox->currentText();
		if ("Data" == whatOrHowToBuild ){
			bs->BuildImage(om::NON_BLOCKING);

		} else if ( "Mesh" == whatOrHowToBuild ){
			bs->BuildMesh(om::NON_BLOCKING);

		} else if ("Data & Mesh" == whatOrHowToBuild){
			bs->BuildAndMeshSegmentation(om::NON_BLOCKING);

		} else if ("Load Dendrogram" == whatOrHowToBuild){
			bs->loadDendrogram();

		} else if( "Blank Volume" == whatOrHowToBuild ){
			bs->buildBlankVolume();
		}
	}
};

#endif

#ifndef SEG_INSPECTOR_MESH_PREVIEW_BUTTON_H
#define SEG_INSPECTOR_MESH_PREVIEW_BUTTON_H

#include "gui/widgets/omButton.h"
#include "gui/inspectors/segmentation/segInspector.h"
#include "gui/meshPreviewer/meshPreviewer.hpp"

class MeshPreviewButton : public OmButton<SegInspector> {
public:
	MeshPreviewButton(SegInspector * d)
		: OmButton<SegInspector>( d,
								  "Mesh Preview",
								  "preview mesh",
								  false)
	{
	}

private:
	void doAction()
	{
		boost::shared_ptr<SegmentationDataWrapper> sdw =
			mParent->getSegmentationDataWrapper();

		new MeshPreviewer(this, sdw, mParent->getViewGroupState());
	}
};

#endif

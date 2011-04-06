#ifndef SEG_INSPECTOR_MESH_PREVIEW_BUTTON_H
#define SEG_INSPECTOR_MESH_PREVIEW_BUTTON_H

#include "gui/widgets/omButton.hpp"
#include "gui/inspectors/segmentation/segmentationInspector.h"
#include "gui/meshPreviewer/meshPreviewer.hpp"

class MeshPreviewButton : public OmButton<SegmentationInspector> {
public:
	MeshPreviewButton(SegmentationInspector * d)
		: OmButton<SegmentationInspector>( d,
								  "Mesh Preview",
								  "preview mesh",
								  false)
	{}

private:
	void doAction()
	{
		const SegmentationDataWrapper& sdw = mParent->GetSDW();

		new MeshPreviewer(this, sdw, mParent->getViewGroupState());
	}
};

#endif

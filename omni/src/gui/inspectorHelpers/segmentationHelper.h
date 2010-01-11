#ifndef INSPECTOR_HELPER_SEGMENTATION_HELPER
#define INSPECTOR_HELPER_SEGMENTATION_HELPER

#include"../ui_segInspector.h"
#include"../segInspector.h"

#include "inspectorHelper.h"

class SegmentationHelper : public InspectorHelper<SegmentationDataWrapper> {

 Q_OBJECT

 public:
	SegmentationHelper( MyInspectorWidget* parent );
	void addToSplitter( SegmentationDataWrapper data );

 private:
	SegInspector *segInspectorWidget;
	void populateSegmentationInspector(OmId s_id);
};

#endif

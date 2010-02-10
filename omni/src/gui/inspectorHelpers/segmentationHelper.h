#ifndef INSPECTOR_HELPER_SEGMENTATION_HELPER
#define INSPECTOR_HELPER_SEGMENTATION_HELPER

#include"../segInspector.h"

#include "inspectorHelper.h"

class SegmentationHelper : public InspectorHelper<SegmentationDataWrapper> {

 Q_OBJECT

 public:
	SegmentationHelper( MyInspectorWidget* parent );

};

#endif

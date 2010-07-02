#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

ThresholdGroup::ThresholdGroup(DendToolBar * d)
	: OmThresholdGroup(d, "Overall Threshold")
{
	setSingleStep(0.02);
	setMaximum(1.0);
	setInitialGUIThresholdValue();
}

void ThresholdGroup::actUponThresholdChange( const float threshold )
{
	SegmentationDataWrapper sdw = mDendToolbar->getSegmentationDataWrapper();
	if(!sdw.isValid()){
		return;
	}
	
	sdw.getSegmentation().SetDendThresholdAndReload(threshold);
	OmEvents::SegmentModified();
}

void ThresholdGroup::setInitialGUIThresholdValue()
{
	double threshold = 0.95;

	SegmentationDataWrapper sdw = mDendToolbar->getSegmentationDataWrapper();
	if(sdw.isValid()){
		threshold = sdw.getSegmentation().GetDendThreshold();
	} 

        setGUIvalue(threshold);
}

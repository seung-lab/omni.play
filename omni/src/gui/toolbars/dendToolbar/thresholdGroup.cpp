#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

ThresholdGroup::ThresholdGroup(DendToolBar * d)
	: OmThresholdGroup(d, "Overall Threshold")
{
	setThresholdValue();
}

float ThresholdGroup::getThresholdEpsilon()
{
	return 0.02;
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

void ThresholdGroup::setThresholdValue()
{
	QString value;

	SegmentationDataWrapper sdw = mDendToolbar->getSegmentationDataWrapper();
	if(sdw.isValid()){
		value.setNum( sdw.getSegmentation().GetDendThreshold() );
	} else {
		value.setNum(0.95);
	}
	mThreshold->setText(value);
}

float ThresholdGroup::filterUserEnteredThreshold(const float threshold)
{
	float ret = threshold;

	if(threshold > 1.0) {
		ret = 1.0;
	}
	if(threshold < 0.0) {
		ret = 0.0;
	}

	return ret;
}

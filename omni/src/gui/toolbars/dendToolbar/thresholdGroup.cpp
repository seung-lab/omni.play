#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/thresholdGroup.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

static const float thresholdEpsilon = 0.02;

ThresholdGroup::ThresholdGroup(DendToolBar * d)
	: OmThresholdGroup(d, "Overall Threshold")
{
	setThresholdValue();
}

void ThresholdGroup::haveSegmentationChangeThreshold( const float threshold )
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

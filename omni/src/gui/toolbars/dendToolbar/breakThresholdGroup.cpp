#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/breakThresholdGroup.h"
#include "system/viewGroup/omViewGroupState.h"

BreakThresholdGroup::BreakThresholdGroup(DendToolBar * d)
	: OmThresholdGroup(d, "Break Threshold")
{
	setThresholdValue();
}

float BreakThresholdGroup::getThresholdEpsilon()
{
	return 0.02;
}

void BreakThresholdGroup::actUponThresholdChange( const float threshold )
{
	if( NULL != mDendToolbar->getViewGroupState() ) {
		mDendToolbar->getViewGroupState()->setBreakThreshold( threshold );
	}
}

void BreakThresholdGroup::setThresholdValue()
{
	QString value;
	value.setNum(0.95);

	mThreshold->setText(value);

	if( NULL != mDendToolbar->getViewGroupState() ) {
		mDendToolbar->getViewGroupState()->setBreakThreshold( 0.95 );
	}
}

float BreakThresholdGroup::filterUserEnteredThreshold(const float threshold)
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

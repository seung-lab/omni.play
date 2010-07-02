#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/breakThresholdGroup.h"
#include "system/viewGroup/omViewGroupState.h"

BreakThresholdGroup::BreakThresholdGroup(DendToolBar * d)
	: OmThresholdGroup(d, "Break Threshold")
{
	setSingleStep(0.02);
	setMaximum(1.0);
	setInitialGUIThresholdValue();
}

void BreakThresholdGroup::actUponThresholdChange( const float threshold )
{
	if( NULL != mDendToolbar->getViewGroupState() ) {
		mDendToolbar->getViewGroupState()->setBreakThreshold( threshold );
	}
}

void BreakThresholdGroup::setInitialGUIThresholdValue()
{
	const double defaultThreshold = 0.95;

	setGUIvalue(defaultThreshold);

	if( NULL != mDendToolbar->getViewGroupState() ) {
		mDendToolbar->getViewGroupState()->setBreakThreshold(defaultThreshold);
	}
}

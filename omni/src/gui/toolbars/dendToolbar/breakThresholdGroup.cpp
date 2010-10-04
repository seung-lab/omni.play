#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/graphTools.h"
#include "gui/toolbars/dendToolbar/breakThresholdGroup.h"
#include "viewGroup/omViewGroupState.h"

BreakThresholdGroup::BreakThresholdGroup(GraphTools * d)
	: OmThresholdGroup(d, false)
	, mParent(d)
{
	setSingleStep(0.02);
	setMaximum(1.0);
	setInitialGUIThresholdValue();
}

void BreakThresholdGroup::actUponThresholdChange( const float threshold )
{
	if( NULL != mParent->getViewGroupState() ) {
		mParent->getViewGroupState()->setBreakThreshold( threshold );
	}
}

void BreakThresholdGroup::setInitialGUIThresholdValue()
{
	const double defaultThreshold = 0.95;

	setGUIvalue(defaultThreshold);

	if( NULL != mParent->getViewGroupState() ) {
		mParent->getViewGroupState()->setBreakThreshold(defaultThreshold);
	}
}

#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/displayTools.h"
#include "gui/toolbars/dendToolbar/dust3DthresholdGroup.h"
#include "viewGroup/omViewGroupState.h"
#include <limits>

Dust3DThresholdGroup::Dust3DThresholdGroup(DisplayTools * d)
	: OmThresholdGroup(d, true)
	, mParent(d)
{
	setSingleStep(5);
	setMaximum(std::numeric_limits<double>::max());
	setDecimals(0);
	setInitialGUIThresholdValue();
}

void Dust3DThresholdGroup::actUponThresholdChange( const float threshold )
{
	if( NULL != mParent->getViewGroupState() ) {
		mParent->getViewGroupState()->setDustThreshold( threshold );
	}
}

void Dust3DThresholdGroup::setInitialGUIThresholdValue()
{
	float dThreshold = 900;

        if( NULL != mParent->getViewGroupState() ) {
		dThreshold = mParent->getViewGroupState()->getDustThreshold();
	}

        setGUIvalue(dThreshold);
}

#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/dust3DthresholdGroup.h"
#include "system/viewGroup/omViewGroupState.h"
#include <limits>

Dust3DThresholdGroup::Dust3DThresholdGroup(DendToolBar * d)
	: OmThresholdGroup(d, "3D Dust Threshold")
{
	setSingleStep(5);
	setMaximum(std::numeric_limits<double>::max());
	setDecimals(0);
	setInitialGUIThresholdValue();
}

void Dust3DThresholdGroup::actUponThresholdChange( const float threshold )
{
	if( NULL != mDendToolbar->getViewGroupState() ) {
		mDendToolbar->getViewGroupState()->setDustThreshold( threshold );
	}
}

void Dust3DThresholdGroup::setInitialGUIThresholdValue()
{
	float dThreshold = 90;

        if( NULL != mDendToolbar->getViewGroupState() ) {
		dThreshold = mDendToolbar->getViewGroupState()->getDustThreshold();
	}

        setGUIvalue(dThreshold);
}

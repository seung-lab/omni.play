#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/dendToolbar.h"
#include "gui/toolbars/dendToolbar/dust3DthresholdGroup.h"
#include "system/viewGroup/omViewGroupState.h"

Dust3DThresholdGroup::Dust3DThresholdGroup(DendToolBar * d)
	: OmThresholdGroup(d, "3D Dust Threshold")
{
	setThresholdValue();
}

float Dust3DThresholdGroup::getThresholdEpsilon()
{
	return 5;
}

void Dust3DThresholdGroup::actUponThresholdChange( const float threshold )
{
        mDendToolbar->getViewGroupState()->setDustThreshold( threshold );
}

void Dust3DThresholdGroup::setThresholdValue()
{
	float dThreshold = 90;

        if( NULL != mDendToolbar->getViewGroupState() ) {
		dThreshold = mDendToolbar->getViewGroupState()->getDustThreshold();
	}

        mThreshold->setText( QString::number(dThreshold));
}

float Dust3DThresholdGroup::filterUserEnteredThreshold(const float threshold)
{
	float ret = threshold;
	if(threshold < 0.0) {
		ret = 0;
	}

	return ret;
}

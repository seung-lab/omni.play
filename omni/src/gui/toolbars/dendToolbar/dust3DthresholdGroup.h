#ifndef DUST_3D_THRESHOLD_GROUP_H
#define DUST_3D_THRESHOLD_GROUP_H

#include "gui/widgets/omSpinBox.hpp"
#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/displayTools.h"
#include "gui/toolbars/dendToolbar/dust3DthresholdGroup.h"
#include "viewGroup/omViewGroupState.h"
#include <limits>

class Dust3DThresholdGroup : public OmSpinBox {
	Q_OBJECT

public:

	Dust3DThresholdGroup(DisplayTools* d)
		: OmSpinBox(d, om::UPDATE_AS_TYPE)
		, mParent(d)
	{
		setSingleStep(5);
		setMaximum(std::numeric_limits<double>::max());
		setDecimals(0);
		setInitialGUIThresholdValue();
	}

private:
	DisplayTools *const mParent;

	void actUponThresholdChange( const float threshold )
	{
		if( NULL != mParent->getViewGroupState() ) {
			mParent->getViewGroupState()->setDustThreshold( threshold );
		}
	}

	void setInitialGUIThresholdValue()
	{
		float dThreshold = 900;

        if( NULL != mParent->getViewGroupState() ) {
			dThreshold = mParent->getViewGroupState()->getDustThreshold();
		}

        setGUIvalue(dThreshold);
	}

};

#endif

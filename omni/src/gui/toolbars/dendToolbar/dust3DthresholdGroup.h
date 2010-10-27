#ifndef DUST_3D_THRESHOLD_GROUP_H
#define DUST_3D_THRESHOLD_GROUP_H

#include "gui/widgets/omIntSpinBox.hpp"
#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/displayTools.h"
#include "gui/toolbars/dendToolbar/dust3DthresholdGroup.h"
#include "viewGroup/omViewGroupState.h"
#include <limits>

class Dust3DThresholdGroup : public OmIntSpinBox {
	Q_OBJECT

public:
	Dust3DThresholdGroup(DisplayTools* d)
		: OmIntSpinBox(d, om::UPDATE_AS_TYPE)
		, mParent(d)
	{
		setSingleStep(5);
		setMaximum(std::numeric_limits<int32_t>::max());
		setInitialGUIThresholdValue();
	}

private:
	DisplayTools *const mParent;

	void actUponSpinboxChange(const int threshold)
	{
		if( NULL != vgs() ) {
			vgs()->setDustThreshold( threshold );
		}
	}

	void setInitialGUIThresholdValue()
	{
		int dThreshold = 900;

        if( NULL != vgs() ) {
			dThreshold = vgs()->getDustThreshold();
		}

        setGUIvalue(dThreshold);
	}

	OmViewGroupState* vgs() const {
		return mParent->getViewGroupState();
	}
};

#endif

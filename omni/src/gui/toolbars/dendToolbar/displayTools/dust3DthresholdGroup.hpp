#ifndef DUST_3D_THRESHOLD_GROUP_HPP
#define DUST_3D_THRESHOLD_GROUP_HPP

#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/displayTools/displayTools.h"
#include "gui/widgets/omIntSpinBox.hpp"
#include "system/omEvents.h"
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
			OmEvents::Redraw3d();
		}
	}

	void setInitialGUIThresholdValue()
	{
		int dThreshold = 900;

        if( NULL != vgs() ) {
			dThreshold = vgs()->getDustThreshold();
		}

        setValue(dThreshold);
	}

	OmViewGroupState* vgs() const {
		return mParent->getViewGroupState();
	}
};

#endif

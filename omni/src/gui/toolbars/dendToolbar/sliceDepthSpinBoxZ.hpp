#ifndef SLICE_DEPTH_SPIN_BOX_Z_HPP
#define SLICE_DEPTH_SPIN_BOX_Z_HPP

#include "gui/toolbars/dendToolbar/sliceDepthSpinBoxBase.hpp"

class SliceDepthSpinBoxZ : public SliceDepthSpinBoxBase {
	Q_OBJECT

public:

	SliceDepthSpinBoxZ(DisplayTools* d)
		: SliceDepthSpinBoxBase(d)
	{
		setInitialGUIThresholdValue();
	}

private:
	virtual ViewType viewType() const {
		return XY_VIEW;
	}
};

#endif

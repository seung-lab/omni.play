#ifndef SLICE_DEPTH_SPIN_BOX_X_HPP
#define SLICE_DEPTH_SPIN_BOX_X_HPP

#include "gui/toolbars/dendToolbar/sliceDepthSpinBoxBase.hpp"

class SliceDepthSpinBoxX : public SliceDepthSpinBoxBase {
	Q_OBJECT

public:

	SliceDepthSpinBoxX(DisplayTools* d)
		: SliceDepthSpinBoxBase(d)
	{
		setInitialGUIThresholdValue();
	}

private:
	virtual ViewType viewType() const {
		return YZ_VIEW;
	}
};

#endif

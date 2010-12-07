#ifndef SLICE_DEPTH_SPIN_BOX_Y_HPP
#define SLICE_DEPTH_SPIN_BOX_Y_HPP

#include "gui/toolbars/dendToolbar/displayTools/sliceDepthSpinBoxBase.hpp"

class SliceDepthSpinBoxY : public SliceDepthSpinBoxBase {
	Q_OBJECT

public:
	SliceDepthSpinBoxY(DisplayTools* d)
		: SliceDepthSpinBoxBase(d)
	{}

private:
	virtual ViewType viewType() const {
		return XZ_VIEW;
	}
};

#endif

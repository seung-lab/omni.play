#ifndef SLICE_DEPTH_SPIN_BOX_BASE_HPP
#define SLICE_DEPTH_SPIN_BOX_BASE_HPP

#include "common/omDebug.h"
#include "gui/toolbars/dendToolbar/displayTools/displayTools.h"
#include "gui/widgets/omIntSpinBox.hpp"
#include "system/events/omViewEvent.h"
#include "viewGroup/omViewGroupState.h"

#include <limits>

class SliceDepthSpinBoxBase : public OmIntSpinBox,
							  public OmViewEventListener {
Q_OBJECT

public:
	SliceDepthSpinBoxBase(DisplayTools* d)
		: OmIntSpinBox(d, om::UPDATE_AS_TYPE)
		, mParent(d)
	{
		setValue(0);
		setSingleStep(1);
		setMaximum(std::numeric_limits<int32_t>::max());
	}

	QSize sizeHint () const {
		return QSize(50, height());
	}

private:
	DisplayTools *const mParent;

	virtual ViewType viewType() const = 0;

	void actUponSpinboxChange(const int depth)
	{
		if(NULL == vgs()){
			return;
		}

		vgs()->SetViewSliceDepth(viewType(), depth);
		OmEvents::Redraw2d();
	}

	void update()
	{
		blockSignals(true);

		const int depth = vgs()->GetViewSliceDepth(viewType());
		setValue(depth);

		blockSignals(false);
	}

	inline OmViewGroupState* vgs() const {
		return mParent->getViewGroupState();
	}

// OmViewEventListener stuff
	void ViewBoxChangeEvent(){
		update();
	}
	void ViewPosChangeEvent(){}
	void ViewCenterChangeEvent(){}
	void ViewRedrawEvent(){}
};

#endif

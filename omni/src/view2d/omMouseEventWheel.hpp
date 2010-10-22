#ifndef OM_MOUSE_EVENT_WHEEL_HPP
#define OM_MOUSE_EVENT_WHEEL_HPP

#include "gui/toolbars/mainToolbar/filterWidget.h"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2dZoom.hpp"

class OmMouseEventWheel{
private:
	OmView2d *const v2d_;
	boost::shared_ptr<OmView2dState> state_;

public:
	OmMouseEventWheel(OmView2d* v2d,
			    boost::shared_ptr<OmView2dState> state)
		: v2d_(v2d)
		, state_(state)
	{}

	void Wheel(QWheelEvent* event)
	{
		const int numDegrees = event->delta() / 8;
		const int numSteps = numDegrees / 15;

		const bool move_through_stack = event->modifiers() & Qt::ControlModifier;
		const bool change_alpha = event->modifiers() & Qt::ShiftModifier;

		if (move_through_stack) {
			if (numSteps >= 0) {
				state_->MoveUpStackCloserToViewer();
			} else {
				state_->MoveDownStackFartherFromViewer();
			}
		} else if(change_alpha){
			FilterWidget* fw = state_->getViewGroupState()->GetFilterWidget();
			if(fw){
				if (numSteps >= 0) {
					fw->increaseAlpha();
				} else{
					fw->decreaseAlpha();
				}
			}
		} else {
			v2d_->Zoom()->MouseWheelZoom(numSteps);
		}

		event->accept();

		OmEvents::Redraw3d();
	}
};
#endif

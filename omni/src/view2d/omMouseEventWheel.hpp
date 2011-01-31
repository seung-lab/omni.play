#ifndef OM_MOUSE_EVENT_WHEEL_HPP
#define OM_MOUSE_EVENT_WHEEL_HPP

#include "gui/toolbars/mainToolbar/filterWidget.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2dZoom.hpp"

class OmMouseEventWheel{
private:
	OmView2d *const v2d_;
	OmView2dState *const state_;

public:
	OmMouseEventWheel(OmView2d* v2d, OmView2dState* state)
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
			if (numSteps >= 0) {
				FilterWidget::IncreaseAlpha();
			} else{
				FilterWidget::DecreaseAlpha();
			}
		} else {
			v2d_->Zoom()->MouseWheelZoom(numSteps);
		}

		event->accept();

		OmEvents::Redraw3d();
	}
};
#endif

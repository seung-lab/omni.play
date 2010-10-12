#ifndef OM_MOUSE_ZOOM_HPP
#define OM_MOUSE_ZOOM_HPP

#include "view2d/omView2dState.hpp"

#include <QMouseEvent>

class OmMouseZoom{
public:
	static void MouseZoom(boost::shared_ptr<OmView2dState> state,
						  QMouseEvent * event)
	{
		const bool zoomOut = event->modifiers() & Qt::ControlModifier;

		if (zoomOut) {
			state->MouseWheelZoom(-15);
		} else {
			state->MouseWheelZoom(15);
		}
	}
};

#endif

#ifndef OM_MOUSE_ZOOM_HPP
#define OM_MOUSE_ZOOM_HPP

#include "view2d/omView2dState.hpp"

#include <QMouseEvent>

class OmMouseZoom{
public:
	static void MouseZoomIn(boost::shared_ptr<OmView2dState> state){
		MouseWheelZoom(state, 15);
	}

	static void MouseZoomOut(boost::shared_ptr<OmView2dState> state){
		MouseWheelZoom(state, -15);
	}

	static void MouseZoom(boost::shared_ptr<OmView2dState> state,
			      QMouseEvent * event)
	{
		const bool zoomOut = event->modifiers() & Qt::ControlModifier;

		if (zoomOut) {
			MouseZoomOut(state);
		} else {
			MouseZoomIn(state);
		}
	}

	static void MouseWheelZoom(boost::shared_ptr<OmView2dState> state,
							   const int numSteps)
	{
		const int curMipLevel = state->getMipLevel();
		const float curZoom = state->getZoomScale();
		int mipLevel;
		int zoomFactor;

		if (numSteps >= 0) { // ZOOMING IN
			if(!state->IsLevelLocked() && curZoom >= 1 && curMipLevel > 0){
				//move to previous mip level
				mipLevel = curMipLevel - 1;
				zoomFactor = 6;
			} else{
				mipLevel = curMipLevel;
				zoomFactor = ceil(curZoom * 10.0 + (1 * numSteps));
			}
		} else { // ZOOMING OUT
			if (!state->IsLevelLocked() && curZoom <= 0.6 &&
			    curMipLevel < state->getVol()->GetRootMipLevel()){
				// need to move to next mip level
				mipLevel = curMipLevel + 1;
				zoomFactor = 10;
			} else if (curZoom > 0.1) {
				int zoom = curZoom * 10 - (1 * (-1 * numSteps));
				if (zoom < 1) {
					zoom = 1;
				}
				mipLevel = curMipLevel;
				zoomFactor = zoom;
			}else{
				return;
			}
		}

		state->PanAndZoom(mipLevel, zoomFactor);
	}
};

#endif

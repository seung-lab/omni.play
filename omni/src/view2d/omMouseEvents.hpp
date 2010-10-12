#ifndef OM_MOUSE_EVENTS_HPP
#define OM_MOUSE_EVENTS_HPP

#include "view2d/omMouseEventMove.hpp"
#include "view2d/omMouseEventPress.hpp"
#include "view2d/omMouseEventRelease.hpp"
#include "view2d/omMouseEventWheel.hpp"
#include "view2d/omView2dZoom.hpp"

#include <boost/make_shared.hpp>

class OmMouseEvents{
private:
	boost::shared_ptr<OmMouseEventMove> move_;
	boost::shared_ptr<OmMouseEventPress> press_;
	boost::shared_ptr<OmMouseEventRelease> release_;
	boost::shared_ptr<OmMouseEventWheel> wheel_;

public:
	OmMouseEvents(OmView2d* v2d,
		      boost::shared_ptr<OmView2dState> state)
		: move_(boost::make_shared<OmMouseEventMove>(v2d, state))
		, press_(boost::make_shared<OmMouseEventPress>(v2d, state))
		, release_(boost::make_shared<OmMouseEventRelease>(v2d, state))
		, wheel_(boost::make_shared<OmMouseEventWheel>(v2d, state))
	{}

	void Press(QMouseEvent* event){
		press_->Press(event);
	}

	void Move(QMouseEvent* event){
		move_->Move(event);
	}

	void Release(QMouseEvent* event){
		release_->Release(event);
	}

	void Wheel(QWheelEvent* event){
		wheel_->Wheel(event);
	}
};

#endif

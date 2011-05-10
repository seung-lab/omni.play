#pragma once

#include "view2d/omMouseEventMove.hpp"
#include "view2d/omMouseEventPress.hpp"
#include "view2d/omMouseEventRelease.hpp"
#include "view2d/omMouseEventWheel.hpp"
#include "view2d/omView2dZoom.hpp"

#include <boost/make_shared.hpp>

class OmMouseEvents{
private:
    boost::scoped_ptr<OmMouseEventMove> move_;
    boost::scoped_ptr<OmMouseEventPress> press_;
    boost::scoped_ptr<OmMouseEventRelease> release_;
    boost::scoped_ptr<OmMouseEventWheel> wheel_;

public:
    OmMouseEvents(OmView2d* v2d, OmView2dState* state)
        : move_(new OmMouseEventMove(v2d, state))
        , press_(new OmMouseEventPress(v2d, state))
        , release_(new OmMouseEventRelease(v2d, state))
        , wheel_(new OmMouseEventWheel(v2d, state))
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


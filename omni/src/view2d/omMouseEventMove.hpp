#ifndef OM_MOUSE_EVENT_MOVE_HPP
#define OM_MOUSE_EVENT_MOVE_HPP

#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentSelector.h"
#include "utility/dataWrappers.h"
#include "view2d/brush/omBrushSelect.hpp"
#include "view2d/brush/omBrushPaint.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"

class OmMouseEventMove{
private:
    OmView2d *const v2d_;
    OmView2dState *const state_;

    bool controlKey_;
    bool altKey_;
    bool shiftKey_;
    bool leftMouseButton_;
    bool rightMouseButton_;
    om::tool::mode tool_;
    QMouseEvent* event_;
    DataCoord dataClickPoint_;

    friend class OmMouseEventState;

public:
    OmMouseEventMove(OmView2d* v2d, OmView2dState* state)
        : v2d_(v2d)
        , state_(state)
    {}

    void Move(QMouseEvent* event)
    {
        setState(event);

        state_->SetMousePoint(event);

        if(leftMouseButton_){
            switch(tool_){
            case om::tool::SPLIT:
            case om::tool::CUT:
                break;

            case om::tool::SELECT:
                if(controlKey_){ // pan in select-mode
                    mousePan();
                    OmEvents::Redraw3d();
                } else {
                    selectSegments();
                }
                break;

            case om::tool::PAN:
                mousePan();
                OmEvents::Redraw3d();

                break;

            case om::tool::CROSSHAIR:
            case om::tool::ZOOM:
            case om::tool::FILL:
                break;

            case om::tool::PAINT:
            case om::tool::ERASE:
                if (state_->getScribbling()) {
                    // keep painting
                    paint();
                }
                break;
            }
        }

        v2d_->myUpdate();
    }

private:
    inline void setState(QMouseEvent* event){
        OmMouseEventState::SetState(this, state_, event);
    }

    inline void selectSegments()
    {
        if(altKey_){
            OmBrushSelect::SelectByLine(state_, dataClickPoint_, om::SUBTRACT);
        } else {
            OmBrushSelect::SelectByLine(state_, dataClickPoint_, om::ADD);
        }

        state_->SetLastDataPoint(dataClickPoint_);
    }

    void paint()
    {
        SegmentDataWrapper sdw = OmSegmentSelected::Get();
        if (!sdw.IsSegmentValid()){
            return;
        }

        OmSegID segmentValueToPaint = 0;
        if(om::tool::PAINT == tool_){
            segmentValueToPaint = sdw.getID();
        }

        OmBrushPaint::PaintByClick(state_,
                                   dataClickPoint_,
                                   segmentValueToPaint);

        OmBrushPaint::PaintByLine(state_,
                                  dataClickPoint_,
                                  segmentValueToPaint);

        state_->SetLastDataPoint(dataClickPoint_);
    }

    inline void mousePan(){
        state_->DoMousePan(Vector2i(event_->x(), event_->y()));
    }
};

#endif

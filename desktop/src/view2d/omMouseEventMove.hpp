#pragma once

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
        
        if(leftMouseButton_)
        {
            if(shouldPan()){
                doPan();
                v2d_->Redraw();
                return;
            }
            
            switch(tool_)
            {
            case om::tool::SELECT:
                selectSegments();
                break;

            case om::tool::PAN:
                doPan();
                break;

            case om::tool::PAINT:
                if(state_->getScribbling()) {
                    paint();
                }
                state_->SetLastDataPoint(dataClickPoint_);
                break;

            case om::tool::ERASE:
                if (state_->getScribbling()) {
                    erase();
                }
                state_->SetLastDataPoint(dataClickPoint_);
                break;
            default:
                break;
            }
        }

        v2d_->Redraw();
    }

private:
    inline bool shouldPan() const {
        return controlKey_ || state_->OverrideToolModeForPan();
    }

    inline void doPan()
    {
        mousePan();
        OmEvents::Redraw3d();
    }

    inline void setState(QMouseEvent* event)
    {
        event_ = event;

        controlKey_ = event->modifiers() & Qt::ControlModifier;
        shiftKey_ = event->modifiers() & Qt::ShiftModifier;
        altKey_ = event->modifiers() & Qt::AltModifier;

        leftMouseButton_ = event->buttons() & Qt::LeftButton;
        rightMouseButton_ = event->buttons() & Qt::RightButton;

        tool_ = OmStateManager::GetToolMode();
        dataClickPoint_ = state_->ComputeMouseClickPointDataCoord(event);
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

    void erase()
    {
        OmBrushPaint::PaintByClick(state_, dataClickPoint_, 0);
        OmBrushPaint::PaintByLine(state_, dataClickPoint_, 0);
    }

    void paint()
    {
        const OmSegID segmentValueToPaint = state_->GetSegIDForPainting();

        assert(segmentValueToPaint);

        OmBrushPaint::PaintByClick(state_, dataClickPoint_, segmentValueToPaint);

        OmBrushPaint::PaintByLine(state_, dataClickPoint_, segmentValueToPaint);
    }

    inline void mousePan(){
        state_->DoMousePan(Vector2i(event_->x(), event_->y()));
    }
};


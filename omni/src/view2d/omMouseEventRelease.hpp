#ifndef OM_MOUSE_EVENT_RELEASE_HPP
#define OM_MOUSE_EVENT_RELEASE_HPP

#include "gui/segmentLists/elementListBox.hpp"
#include "view2d/omFillTool.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"

class OmMouseEventRelease{
private:
    OmView2d *const v2d_;
    OmView2dState *const state_;

public:
    OmMouseEventRelease(OmView2d* v2d, OmView2dState* state)
        : v2d_(v2d)
        , state_(state)
    {}

    void Release(QMouseEvent* event)
    {
        state_->setScribbling(false);

        switch (OmStateManager::GetToolMode()) {
        case om::tool::SPLIT:
        case om::tool::CUT:
            break;

        case om::tool::SELECT:
            ElementListBox::PopulateLists();
            break;

        case om::tool::CROSSHAIR:
        case om::tool::PAN:
        case om::tool::ZOOM:
            break;

        case om::tool::PAINT:
        case om::tool::ERASE:
        case om::tool::FILL:
            doRelease(event);
            break;
        }
    }

private:
    void doRelease(QMouseEvent* event)
    {
        // END PAINTING

        if (event->button() != Qt::LeftButton) {
            return;
        }

        if(v2d_->amInFillMode())
        {
            MouseRelease_LeftButton_Filling(event);
            return;
        }

        if(!state_->getScribbling()) {
            return;
        }

        state_->setScribbling(false);

        const DataCoord dataClickPoint =
            state_->ComputeMouseClickPointDataCoord(event);

        SegmentDataWrapper sdw = OmSegmentSelected::Get();
        if (!sdw.IsSegmentValid()){
            return;
        }

        switch (OmStateManager::GetToolMode()) {
        case om::tool::PAINT:
            OmBrushPaint::PaintByClick(state_,
                                       dataClickPoint,
                                       sdw.getID());
            break;
        case om::tool::ERASE:
            OmBrushPaint::PaintByClick(state_,
                                       dataClickPoint,
                                       0);
            break;
        default:
            return;
        }

        state_->SetLastDataPoint(dataClickPoint);

        v2d_->myUpdate();
    }

    void MouseRelease_LeftButton_Filling(QMouseEvent * event)
    {
        state_->setScribbling(false);

        const DataCoord dataClickPoint =
            state_->ComputeMouseClickPointDataCoord(event);

        //store current selection
        SegmentDataWrapper sdw = OmSegmentSelected::Get();

        //return if not valid
        if (!sdw.IsSegmentValid() ){
            return;
        }

        if(om::tool::FILL != OmStateManager::GetToolMode()){
            return;
        }

        const OmSegID data_value = sdw.getID();

        const OmSegID segid = sdw.GetSegmentation().GetVoxelValue(dataClickPoint);
        if(!segid){
            return;
        }

        const OmSegID rootSegID = sdw.Segments()->findRootID(segid);

        OmFillTool fillTool(sdw.GetSegmentationPtr(), state_->getViewType(),
                            data_value, rootSegID);
        fillTool.Fill(dataClickPoint);

        v2d_->doRedraw2d();
    }
};

#endif

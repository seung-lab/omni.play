#ifndef OM_MOUSE_EVENT_PRESS_HPP
#define OM_MOUSE_EVENT_PRESS_HPP

#include "view2d/omBrushSelect.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "gui/widgets/omSegmentContextMenu.h"
#include "actions/omActions.h"

class OmMouseEventPress{
private:
    OmView2d *const v2d_;
    OmView2dState *const state_;

    OmSegmentContextMenu mSegmentContextMenu;

    bool controlKey_;
    bool altKey_;
    bool shiftKey_;
    bool leftMouseButton_;
    bool rightMouseButton_;
    OmToolMode tool_;
    QMouseEvent* event_;
    DataCoord dataClickPoint_;

    friend class OmMouseEventState;

public:
    OmMouseEventPress(OmView2d* v2d, OmView2dState* state)
        : v2d_(v2d)
        , state_(state)
    {}

    void Press(QMouseEvent* event)
    {
        setState(event);

        state_->SetClickPoint(event->x(), event->y());

        if(leftMouseButton_){
            if(SPLIT_MODE == tool_){
                doFindAndSplitSegment();
                v2d_->doRedraw2d();
                return;
            }

            if(CUT_MODE == tool_){
                doFindAndCutSegment();
                v2d_->doRedraw2d();
                return;
            }

            const bool doCrosshair = controlKey_ && SELECT_MODE != tool_;

            if(doCrosshair){
                mouseSetCrosshair();

            } else {
                mouseLeftButton();
            }

        } else if(rightMouseButton_){
            if(controlKey_){
                mouseSelectSegment();

            } else {
                mouseShowSegmentContextMenu();
            }
        }

        state_->SetCameraMoving(true);
    }

private:
    void setState(QMouseEvent* event){
        OmMouseEventState::SetState(this, state_, event);
    }

    void doFindAndSplitSegment()
    {
        boost::optional<SegmentDataWrapper> sdw = getSelectedSegment();

        if(!sdw) {
            return;
        }

        OmActions::FindAndSplitSegments(*sdw,
                                        state_->getViewGroupState(),
                                        dataClickPoint_);
    }

    void doFindAndCutSegment()
    {
        boost::optional<SegmentDataWrapper> sdw = getSelectedSegment();

        if(!sdw) {
            return;
        }

        OmActions::FindAndCutSegments(*sdw, state_->getViewGroupState());
    }

    void mouseSetCrosshair()
    {
        v2d_->doRedraw2d();
        v2d_->SetDepth(event_);
        state_->getViewGroupState()->setTool(PAN_MODE);
    }

    void mouseLeftButton()
    {
        switch(tool_){
        case SELECT_MODE:
            if(controlKey_){
                return;
            } else {
                state_->setScribbling(true);
                selectSegments();
            }
            break;
        case PAN_MODE:
            return;
        case CROSSHAIR_MODE:
            mouseSetCrosshair();
            return;
        case ZOOM_MODE:
            v2d_->Zoom()->MouseLeftButtonClick(event_);
            OmEvents::Redraw3d();
            return;
        case ADD_VOXEL_MODE:
            state_->setScribbling(true);
            paint();
            break;
        case SUBTRACT_VOXEL_MODE:
            state_->setScribbling(true);
            paint();
            break;
        default:
            return;
        }

        state_->SetLastDataPoint(dataClickPoint_);

        v2d_->myUpdate();
    }

    void selectSegments()
    {
        om::AddOrSubtract addOrSubtractSegments = om::ADD;
        if(altKey_){
            addOrSubtractSegments = om::SUBTRACT;
        }

        OmBrushSelect::SelectByClick(state_, dataClickPoint_,
                                     addOrSubtractSegments);
    }

    void paint()
    {
        SegmentDataWrapper sdw = OmSegmentSelected::Get();
        if(!sdw.IsSegmentValid()){
            return;
        }

        OmSegID segmentValueToPaint = 0;
        if(ADD_VOXEL_MODE == tool_){
            segmentValueToPaint = sdw.getID();
        }

        OmBrushPaint::PaintByClick(state_,
                                   dataClickPoint_,
                                   segmentValueToPaint);
    }

    void mouseSelectSegment()
    {
        boost::optional<SegmentDataWrapper> sdw = getSelectedSegment();
        if(!sdw){
            return;
        }

        const bool augment_selection = shiftKey_;
        return doSelectSegment(*sdw, augment_selection );
    }

    void doSelectSegment(const SegmentDataWrapper& sdw, bool augment_selection )
    {
        if( !sdw.IsSegmentValid() ){
            printf("not valid\n");
            return;
        }

        const OmID segmentID = sdw.getID();

        OmSegmentSelected::Set(sdw);

        OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(), this, "view2dEvent" );
        if( augment_selection ){
            sel.augmentSelectedSet_toggle( segmentID);
        } else {
            sel.selectJustThisSegment_toggle( segmentID );
        }
        sel.sendEvent();

        state_->touchFreshnessAndRedraw2d();
        v2d_->myUpdate();
    }

    void mouseShowSegmentContextMenu()
    {
        boost::optional<SegmentDataWrapper> sdw = getSelectedSegment();

        if(sdw){
            mSegmentContextMenu.Refresh(*sdw, state_->getViewGroupState(),
                                        dataClickPoint_);
            mSegmentContextMenu.exec(event_->globalPos());
        }
    }

    boost::optional<SegmentDataWrapper> getSelectedSegment()
    {
        if(SEGMENTATION == state_->getVol()->getVolumeType()){
            SegmentationDataWrapper sdw(state_->getVol()->getID());
            return getSelectedSegmentSegmentation(sdw);
        }

        boost::optional<SegmentDataWrapper> ret;

        ChannelDataWrapper cdw(state_->getVol()->getID());
        const std::vector<OmFilter2d*> filters = cdw.GetFilters();

        FOR_EACH(iter, filters){
            OmFilter2d* filter = *iter;
            SegmentationDataWrapper sdw = filter->GetSegmentationWrapper();
            if (!sdw.IsSegmentationValid()){
                continue;
            }

            ret = getSelectedSegmentSegmentation(sdw);

            if(ret){
                break;
            }
        }

        return ret;
    }

    boost::optional<SegmentDataWrapper>
    getSelectedSegmentSegmentation(const SegmentationDataWrapper& sdw)
    {
        const OmSegID segmentID =
            sdw.GetSegmentation().GetVoxelValue(dataClickPoint_);

        printf("id = %u\n", segmentID);

        if(0 == segmentID){
            return boost::optional<SegmentDataWrapper>();
        }

        SegmentDataWrapper ret(sdw, segmentID);
        return boost::optional<SegmentDataWrapper>(ret);
    }
};

#endif

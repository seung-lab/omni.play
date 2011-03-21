#ifndef OM_MOUSE_EVENT_PRESS_HPP
#define OM_MOUSE_EVENT_PRESS_HPP

#include "gui/widgets/omTellInfo.hpp"
#include "actions/omActions.h"
#include "gui/widgets/omSegmentContextMenu.h"
#include "view2d/brush/omBrushSelect.hpp"
#include "view2d/omMouseEventUtils.hpp"
#include "view2d/omView2d.h"
#include "view2d/omView2dState.hpp"

class OmMouseEventPress{
private:
    OmView2d *const v2d_;
    OmView2dState *const state_;

    OmSegmentContextMenu segmentContextMenu_;

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
    OmMouseEventPress(OmView2d* v2d, OmView2dState* state)
        : v2d_(v2d)
        , state_(state)
    {}

    void Press(QMouseEvent* event)
    {
        setState(event);

        state_->SetMousePanStartingPt(Vector2f(event->x(), event->y()));

        if(leftMouseButton_){
            if(om::tool::SPLIT == tool_){
                doFindAndSplitSegment();
                v2d_->doRedraw2d();
                return;
            }

            if(om::tool::CUT == tool_){
                doFindAndCutSegment();
                v2d_->doRedraw2d();
                return;
            }

            const bool doCrosshair = controlKey_ && om::tool::PAN == tool_;

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
        setDepth();
        state_->getViewGroupState()->setTool(om::tool::PAN);
    }

    void setDepth()
    {
        const ScreenCoord screenc = ScreenCoord(event_->x(), event_->y());
        const DataCoord newDepth = state_->ScreenToDataCoord(screenc);
        state_->Location()->SetDataLocation(newDepth);

        OmEvents::ViewCenterChanged();
    }

    void mouseLeftButton()
    {
        switch(tool_){
        case om::tool::SELECT:
            if(controlKey_){
                return;
            } else {
                state_->setScribbling(true);
                selectSegments();
            }
            break;
        case om::tool::PAN:
            return;
        case om::tool::CROSSHAIR:
            mouseSetCrosshair();
            return;
        case om::tool::ZOOM:
            v2d_->Zoom()->MouseLeftButtonClick(event_);
            OmEvents::Redraw3d();
            return;
        case om::tool::PAINT:
            state_->setScribbling(true);
            paint();
            break;
        case om::tool::ERASE:
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
        if(!sdw.IsSegmentValid())
        {
            OmTellInfo("Please select a segment");
            return;
        }

        OmSegID segmentValueToPaint = 0;
        if(om::tool::PAINT == tool_){
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
            segmentContextMenu_.Refresh(*sdw, state_->getViewGroupState(),
                                        dataClickPoint_);
            segmentContextMenu_.exec(event_->globalPos());
        }
    }

    boost::optional<SegmentDataWrapper> getSelectedSegment()
    {
        OmMipVolume* vol = state_->getVol();

        if(SEGMENTATION == vol->getVolumeType())
        {
            OmSegmentation* seg = reinterpret_cast<OmSegmentation*>(vol);
            return getSelectedSegmentSegmentation(seg);
        }

        boost::optional<SegmentDataWrapper> ret;

        OmChannel* chan = reinterpret_cast<OmChannel*>(vol);
        const std::vector<OmFilter2d*> filters = chan->GetFilters();

        FOR_EACH(iter, filters)
        {
            OmFilter2d* filter = *iter;

            if(om::OVERLAY_SEGMENTATION == filter->FilterType())
            {
                ret = getSelectedSegmentSegmentation(filter->GetSegmentation());

                if(ret){
                    break;
                }
            }
        }

        return ret;
    }

    boost::optional<SegmentDataWrapper>
    getSelectedSegmentSegmentation(OmSegmentation* segmentation)
    {
        const OmSegID segmentID = segmentation->GetVoxelValue(dataClickPoint_);

        if(!segmentID){
            return boost::optional<SegmentDataWrapper>();
        }

        SegmentDataWrapper ret(segmentation, segmentID);
        return boost::optional<SegmentDataWrapper>(ret);
    }
};

#endif

#ifndef OM_BRUSH_SELECT_CIRCLE_HPP
#define OM_BRUSH_SELECT_CIRCLE_HPP

#include "segment/omSegmentSelector.h"
#include "system/omEvents.h"
#include "view2d/omView2dManager.hpp"
#include "view2d/omBrushOppCircle.hpp"

class OmBrushSelectCircle : public OmBrushOppCircle {
private:
    boost::unordered_set<OmSegID> segIDs_;

    om::AddOrSubtract addOrSubract_;

public:
    OmBrushSelectCircle(OmSegmentation* segmentation,
                        const ViewType viewType,
                        const int brushDia,
                        const int depth,
                        const std::vector<om::point2d>& ptsInCircle)
        : OmBrushOppCircle(segmentation, viewType, brushDia, depth, ptsInCircle)
        , addOrSubract_(om::ADD)
    {}

    virtual ~OmBrushSelectCircle()
    {}

    void SetAddOrSubtract(const om::AddOrSubtract addOrSubract){
        addOrSubract_ = addOrSubract;
    }

    void SelectAndSendEvents(const DataCoord& coord)
    {
        CircleOpp(coord);
        SendEvents();
    }

    void SendEvents()
    {
        doSendEvent();
        OmEvents::Redraw2d();
    }

private:
    inline void voxelOpp(const DataCoord& xyzCoord)
    {
        if( xyzCoord.x < 0 ||
            xyzCoord.y < 0 ||
            xyzCoord.z < 0)
        {
            return;
        }

        const OmSegID segID = segmentation_->GetVoxelValue(xyzCoord);

        if(segID) {
            segIDs_.insert(segID);
        }
    }

    void doSendEvent()
    {
        OmSegmentSelector selector(segmentation_->getSDW(),
                                   this, "view2d_selector");

        selector.ShouldScroll(false);
        selector.AddToRecentList(false);
        selector.AutoCenter(false);
        selector.AugmentListOnly(true);
        selector.AddOrSubtract(addOrSubract_);

        if(om::ADD == addOrSubract_){
            selector.InsertSegments(segIDs_);
        } else {
            selector.RemoveSegments(segIDs_);
        }

        selector.sendEvent();
    }
};

#endif

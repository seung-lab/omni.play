#ifndef OM_BRUSH_SELECT_CIRCLE_HPP
#define OM_BRUSH_SELECT_CIRCLE_HPP

#include "segment/omSegmentSelector.h"
#include "system/omEvents.h"
#include "view2d/omView2dManager.hpp"
#include "view2d/omBrushOppCircle.hpp"
#include "view2d/omSliceCache.hpp"

class OmBrushSelectCircle : public OmBrushOppCircle {
private:
    const DataBbox& segDataExtent_;
    const int chunkDim_;
    boost::scoped_ptr<OmSliceCache> sliceCache_;

    boost::unordered_set<OmSegID> segIDs_;
    om::AddOrSubtract addOrSubract_;

public:
    OmBrushSelectCircle(OmSegmentation* segmentation,
                        const ViewType viewType,
                        const int brushDia,
                        const int depth,
                        const std::vector<om::point2d>& ptsInCircle)
        : OmBrushOppCircle(segmentation, viewType, brushDia, depth, ptsInCircle)
        , segDataExtent_(segmentation->Coords().GetDataExtent())
        , chunkDim_(segmentation->Coords().GetChunkDimension())
        , sliceCache_(new OmSliceCache(segmentation, viewType))
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
        if(!segDataExtent_.contains(xyzCoord)){
            return;
        }


        OmSegID segID = getVoxelValue(xyzCoord);
        //OmSegID segID = segmentation_->GetVoxelValue(xyzCoord);

        if(segID){
            segIDs_.insert(segID);
        }
    }

    inline OmSegID getVoxelValue(const DataCoord& xyzCoord)
    {
        const OmChunkCoord chunkCoord(0,
                                      xyzCoord.x / chunkDim_,
                                      xyzCoord.y / chunkDim_,
                                      xyzCoord.z / chunkDim_);

        const Vector3i chunkPos(xyzCoord.x % chunkDim_,
                                xyzCoord.y % chunkDim_,
                                xyzCoord.z % chunkDim_);

        return sliceCache_->GetVoxelValue(chunkCoord, chunkPos);
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

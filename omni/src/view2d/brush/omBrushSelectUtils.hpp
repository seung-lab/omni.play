#ifndef OM_BRUSH_SELECT_UTILS_HPP
#define OM_BRUSH_SELECT_UTILS_HPP

#include "events/omEvents.h"
#include "segment/omSegmentSelector.h"
#include "view2d/omSliceCache.hpp"

class OmBrushSelectUtils {
public:
    boost::shared_ptr<boost::unordered_set<OmSegID> >
    static FindSegIDsFromPoints(OmBrushOppInfo* info,
                                om::pt3d_list_t* pts)
    {
        const int chunkDim = info->chunkDim;
        const int depth = info->depth;
        const DataBbox& segDataExtent = info->segmentation->Coords().GetDataExtent();

        std::map<OmChunkCoord, std::set<Vector3i> > ptsInChunks;

        FOR_EACH(iter, *pts)
        {
            if(!segDataExtent.contains(*iter)){
                continue;
            }

            const OmChunkCoord chunkCoord(0,
                                          iter->x / chunkDim,
                                          iter->y / chunkDim,
                                          iter->z / chunkDim);

            const Vector3i chunkPos(iter->x % chunkDim,
                                    iter->y % chunkDim,
                                    iter->z % chunkDim);


            ptsInChunks[chunkCoord].insert(chunkPos);
        }

        OmSliceCache sliceCache(info->segmentation,
                                info->viewType);

        return sliceCache.GetSegIDs(ptsInChunks, depth % chunkDim);
    }

    void static SendEvent(OmBrushOppInfo* info, boost::unordered_set<OmSegID>* segIDs)
    {
        OmSegmentSelector selector(info->segmentation->GetSDW(),
                                   NULL, "view2d_selector");

        selector.ShouldScroll(false);
        selector.AddToRecentList(false);
        selector.AutoCenter(false);
        selector.AugmentListOnly(true);
        selector.AddOrSubtract(info->addOrSubract);

        if(om::ADD == info->addOrSubract){
            selector.InsertSegments(segIDs);
        } else {
            selector.RemoveSegments(segIDs);
        }

        selector.sendEvent();

        // OmEvents::Redraw2d();
    }
};

#endif

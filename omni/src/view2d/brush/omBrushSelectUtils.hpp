#ifndef OM_BRUSH_SELECT_UTILS_HPP
#define OM_BRUSH_SELECT_UTILS_HPP

#include "events/omEvents.h"
#include "segment/omSegmentSelector.h"
#include "view2d/brush/omChunksAndPts.hpp"

class OmBrushSelectUtils {
public:
    boost::shared_ptr<boost::unordered_set<OmSegID> >
    static FindSegIDsFromPoints(OmBrushOppInfo* info,
                                om::pt3d_list_t* pts)
    {

        const int depthInChunk = info->depth % info->chunkDim;

        OmChunksAndPts chunksAndPts(info->segmentation, info->viewType);

        chunksAndPts.AddAllPtsThatIntersectVol(pts);

        return chunksAndPts.GetSegIDs(depthInChunk);
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

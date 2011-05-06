#ifndef OM_BRUSH_OPP_HPP
#define OM_BRUSH_OPP_HPP

#include "view2d/brush/omBrushOppTypes.h"
#include "utility/dataWrappers.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omBrushSize.hpp"
#include "view2d/omView2dManager.hpp"

class OmBrushOppInfoFactory {
private:
    static int getDepth(const DataCoord& coord, const ViewType viewType){
        return OmView2dConverters::GetViewTypeDepth(coord, viewType);
    }

public:
    /**
     *
     * DataCoord is just used to determine slice depth
     *
     **/
    static om::shared_ptr<OmBrushOppInfo>
    MakeOppInfo(OmView2dState* state, const DataCoord& coord,
                const om::AddOrSubtract addOrSubract)
    {
        SegmentationDataWrapper sdw(state->GetSegmentationID());
        OmSegmentation* segmentation = sdw.GetSegmentationPtr();

        const ViewType viewType = state->getViewType();
        const int brushSize = state->getBrushSize()->Diameter();
        const int depth = getDepth(coord, viewType);

        const std::vector<om::point2di>& ptsInCircle =
            state->getBrushSize()->GetPtsInCircle();

        const int chunkDim = segmentation->Coords().GetChunkDimension();

        return om::make_shared<OmBrushOppInfo>(segmentation,
                                                  viewType,
                                                  brushSize,
                                                  depth,
                                                  ptsInCircle,
                                                  chunkDim,
                                                  addOrSubract);
    }
};

#endif

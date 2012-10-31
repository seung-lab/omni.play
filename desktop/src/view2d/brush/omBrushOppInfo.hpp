#pragma once

#include "view2d/brush/omBrushOppTypes.h"
#include "utility/dataWrappers.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omBrushSize.hpp"
#include "view2d/omView2dManager.hpp"

class OmBrushOppInfoFactory {
public:
    static om::shared_ptr<OmBrushOppInfo>
    MakeOppInfo(OmView2dState* state, const om::coords::Global& coord,
                const om::AddOrSubtract addOrSubract)
    {
        SegmentationDataWrapper sdw(state->GetSegmentationID());
        OmSegmentation* segmentation = sdw.GetSegmentationPtr();

        const ViewType viewType = state->getViewType();
        const int brushSize = state->getBrushSize()->Diameter();
        const int depth = state->getViewTypeDepth(coord);

        const std::vector<om::point2di>& ptsInCircle =
            state->getBrushSize()->GetPtsInCircle();

        return om::make_shared<OmBrushOppInfo>(segmentation,
                                               viewType,
                                               brushSize,
                                               depth,
                                               ptsInCircle,
                                               addOrSubract);
    }
};


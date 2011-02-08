#ifndef OM_BRUSH_OPP_HPP
#define OM_BRUSH_OPP_HPP

#include "utility/dataWrappers.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omBrushSize.hpp"
#include "view2d/omView2dManager.hpp"

class OmBrushOpp {
private:
    static int brushDiameterForMip(OmView2dState* state)
    {
        const float factor = om::pow2int(state->getMipLevel());
        const int brushDia = state->getBrushSize()->Diameter();
        return brushDia * factor;
    }

    static int getDepth(const DataCoord& coord, const ViewType viewType){
        return OmView2dConverters::GetViewTypeDepth(coord, viewType);
    }

public:
    template <typename CIRCLE>
    static boost::shared_ptr<CIRCLE>
    MakeCircleOpp(OmView2dState* state, const DataCoord& coord)
    {
        SegmentationDataWrapper sdw(state->GetSegmentationID());
        OmSegmentation* segmentation = sdw.GetSegmentationPtr();

        const ViewType viewType = state->getViewType();
        const int brushSize = brushDiameterForMip(state);
        const int depth = getDepth(coord, viewType);
        const std::vector<om::point2d>& ptsInCircle =
            state->getBrushSize()->GetPtsInCircle();

        return boost::make_shared<CIRCLE>(segmentation,
                                          viewType,
                                          brushSize,
                                          depth,
                                          ptsInCircle);
    }
};

#endif

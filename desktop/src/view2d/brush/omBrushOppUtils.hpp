#pragma once

#include "view2d/brush/omBrushOppTypes.h"

class OmBrushOppUtils {
public:

    static void GetPts(OmBrushOppInfo* info, om::pt3d_list_t* pts,
                       const DataCoord& xyzCoord, const ViewType viewType)
    {
        if(1 == info->brushDia){
            pts->push_back(xyzCoord);

        } else {
            FOR_EACH(iter, info->ptsInCircle)
            {
                pts->push_back(OmView2dConverters::IncVectorWith2Pts(xyzCoord,
                                                                     iter->x,
                                                                     iter->y,
                                                                     viewType));
            }
        }
    }
};


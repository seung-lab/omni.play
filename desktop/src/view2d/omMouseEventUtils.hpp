#pragma once

#include "view2d/omView2dZoom.hpp"
#include "view2d/omMouseEventState.hpp"

class OmView2d;

class OmMouseEventUtils{
public:
    static void PickToolAddToSelection(const SegmentDataWrapper& sdw,
                                       const DataCoord& dataClickPoint,
                                       OmView2d* v2d)
    {
        const OmSegID segID = sdw.GetVoxelValue(dataClickPoint);
        if (segID) {
            OmSegmentSelector sel(sdw.MakeSegmentationDataWrapper(),
                                  v2d,
                                  "view2dpick" );
            sel.augmentSelectedSet( segID, true );
            sel.sendEvent();
        }
    }
};


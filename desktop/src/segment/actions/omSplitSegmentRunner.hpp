#pragma once

#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omSplitting.hpp"

class OmSplitSegmentRunner {
public:
    static void FindAndSplitSegments(const SegmentDataWrapper curSDW,
                                     OmViewGroupState* vgs,
                                     const om::globalCoord curClickPt)
    {
        const boost::optional<om::globalCoord> prevClickPt = vgs->Splitting()->Coord();

        if(prevClickPt)
        {
            const SegmentDataWrapper prevSDW = vgs->Splitting()->Segment();

            OmSegment* seg1 = prevSDW.GetSegment();
            OmSegment* seg2 = curSDW.GetSegment();

            vgs->Splitting()->ExitSplitModeFixButton();

            if(!seg1 || !seg2) {
                return;
            }

            if(seg1 == seg2)
            {
                std::cout << "can't split--same segment\n";
                return;
            }

            OmActions::FindAndSplitSegments(seg1, seg2);

        } else {
            if(curSDW.IsSegmentValid())
            {
                // set segment to be split later...
                vgs->Splitting()->SetFirstSplitPoint(curSDW, curClickPt);
            }
        }
    }
};

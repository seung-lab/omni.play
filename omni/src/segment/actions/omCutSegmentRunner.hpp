#pragma once

#include "actions/details/omSegmentSplitAction.h"
#include "segment/omFindCommonEdge.hpp"

class OmCutSegmentRunner {
public:
    static bool CutSegmentFromParent(const SegmentDataWrapper& sdw)
    {
        OmSegment* seg = sdw.GetSegment();

        if(!seg->getParent()){
            return false;
        }

        const OmSegmentEdge edge = OmFindCommonEdge::MakeEdge(seg);

        (new OmSegmentSplitAction(sdw, edge))->Run();
    }
};

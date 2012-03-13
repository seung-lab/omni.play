#pragma once

#include "segment/omSegmentEdge.h"

namespace om {
namespace segmentEdge {

    static OmSegmentEdge MakeEdge(){
        return OmSegmentEdge();
    }

    static OmSegmentEdge MakeEdge(OmSegment* childSeg)
    {
        return OmSegmentEdge(childSeg->getParent()->value(),
                             childSeg->value(),
                             childSeg->getThreshold());
    }

} // namespace segmentEdge
} // namespace om

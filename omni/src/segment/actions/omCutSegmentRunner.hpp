#pragma once

#include "actions/details/omSegmentSplitAction.h"
#include "events/omEvents.h"
#include "segment/omFindCommonEdge.hpp"
#include "utility/segmentDataWrapper.hpp"

class OmCutSegmentRunner {
public:
    static boost::optional<OmSegmentEdge> CutSegmentFromParent(const SegmentDataWrapper& sdw)
    {
        OmSegment* seg = sdw.GetSegment();

        if(!seg->getParent())
        {
            OmEvents::NonFatalEvent("segment is root--not yet spltitable");
            return boost::optional<OmSegmentEdge>();
        }

        boost::optional<std::string> notSplittable = sdw.Segments()->IsSegmentSplittable(seg);

        if(notSplittable)
        {
            const QString err = QString::fromStdString(*notSplittable);
            OmEvents::NonFatalEvent(err);
            return boost::optional<OmSegmentEdge>();
        }

        return OmFindCommonEdge::MakeEdge(seg);
    }
};

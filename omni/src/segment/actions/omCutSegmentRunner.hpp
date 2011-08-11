#pragma once

#include "actions/details/omSegmentCutAction.h"
#include "events/omEvents.h"
#include "segment/omSegmentEdgeUtils.hpp"
#include "utility/segmentDataWrapper.hpp"

class OmCutSegmentRunner {
public:

	// TODO: nuke this old method of cutting once new system is tested (purcaro)
	//
    // static void CutSegmentFromParent(const SegmentDataWrapper& sdw)
    // {
    //     OmSegment* seg = sdw.GetSegment();

    //     if(!seg->getParent())
    //     {
    //         OmEvents::NonFatalEvent("segment is root--not yet spltitable");
    //         return;
    //     }

    //     boost::optional<std::string> notSplittable = sdw.Segments()->IsSegmentSplittable(seg);

    //     if(notSplittable)
    //     {
    //         const QString err = QString::fromStdString(*notSplittable);
    //         OmEvents::NonFatalEvent(err);
    //         return;
    //     }

    //     const OmSegmentEdge edge = om::segmentEdge::MakeEdge(seg);

    //     (new OmSegmentSplitAction(sdw.MakeSegmentationDataWrapper(), edge))->Run();
    // }

     static void CutSegmentFromParent(const SegmentDataWrapper& sdw)
     {
         OmSegment* seg = sdw.GetSegment();

         boost::optional<std::string> notCuttable = sdw.Segments()->IsSegmentCuttable(seg);

         if(notCuttable)
         {
             const QString err = QString::fromStdString(*notCuttable);
             OmEvents::NonFatalEvent(err);
             return;
         }

		 (new OmSegmentCutAction(sdw))->Run();
     }
};

#pragma once

#include "actions/details/omSegmentCutAction.h"
#include "events/omEvents.h"
#include "segment/omSegmentEdgeUtils.hpp"
#include "utility/segmentDataWrapper.hpp"

class OmCutSegmentRunner {
public:

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

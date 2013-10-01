#pragma once

#include "actions/details/omSegmentCutAction.h"
#include "events/events.h"
#include "segment/omSegmentEdgeUtils.hpp"
#include "utility/segmentDataWrapper.hpp"

class OmCutSegmentRunner {
 public:

  static void CutSegmentFromParent(const SegmentDataWrapper& sdw) {
    OmSegment* seg = sdw.GetSegment();

    boost::optional<std::string> notCuttable =
        sdw.Segments()->IsSegmentCuttable(seg);

    if (notCuttable) {
      const QString err = QString::fromStdString(*notCuttable);
      om::event::NonFatalEventOccured(err);
      return;
    }

    (new OmSegmentCutAction(sdw))->Run();
  }
};

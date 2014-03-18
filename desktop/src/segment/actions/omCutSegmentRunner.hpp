#pragma once
#include "precomp.h"

#include "actions/details/omSegmentCutAction.h"
#include "events/events.h"
#include "utility/segmentDataWrapper.hpp"

class OmCutSegmentRunner {
 public:
  static void CutSegmentFromParent(const SegmentDataWrapper& sdw) {
    OmSegment* seg = sdw.GetSegment();

    auto notCuttable = sdw.Segments()->IsSegmentCuttable(seg);

    if (notCuttable) {
      const QString err = QString::fromStdString(*notCuttable);
      om::event::NonFatalEventOccured(err);
      return;
    }

    (new OmSegmentCutAction(sdw))->Run();
  }
};

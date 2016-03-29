#pragma once
#include "precomp.h"

#include "segment/types.hpp"
#include "common/logging.h"
//#include "segment/omSegment.h"
//#include "segment/omSegments.h"

namespace om {
namespace segment {

  om::segment::UserEdge FindMinCutEdge(SegmentationDataWrapper segmentationDataWrapper,
      om::common::SegIDSet sources, om::common::SegIDSet sinks) {
    if (sources.empty() || sinks.empty()) {
      log_debugs << "Source or sink empty";
      return;
    }
    OmSegments* segments = segmentationDataWrapper.Segments();
    om::segment::Children& children = segments->Children();
    OmSegment* rootSegment = segments->FindRoot(sources.begin());

    if (!HasRoot(segments, sources, rootSegment
          || !HasRoot(segments, sinks, rootSegment)) {
      log_debugs << "Source and sink do not share the same root seg " <<
        rootSegment->value();
      return;
    }

  }

  bool HasRoot(OmSegments* segments, om::common::SegIDSet segIDSet,
      OmSegment* desiredRoot) {
    for (auto segID : segIDSet) {
      OmSegment* checkingRoot = segments->FindRoot(segID);
      if (checkingRoot != desiredRoot) {
        return false;
      }
    }
    return true;
  }

}
}


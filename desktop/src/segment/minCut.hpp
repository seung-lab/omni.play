#pragma once
#include "precomp.h"

#include "segment/types.hpp"
#include "segment/boostgraph/types.hpp"
#include "common/logging.h"

class OmSegment;
class OmSegments;

using namespace om::segment::boostgraph;

/*
 * This class creates a BoostGraph and asks it for edges necessary for min cut
 * For now we only return the first min cut edge.
 */
class MinCut {
 public:
  MinCut(OmSegments& segments);

  // i.e. for testing
  MinCut(OmSegments& segments, BoostGraph boostGraph);

  // Find the next min cut edge for this source and sink
  om::segment::UserEdge FindEdge(om::common::SegIDSet sources,
      om::common::SegIDSet sinks);

 private:
  OmSegments& segments_;
  BoostGraph boostGraph_;

  bool hasRoot(om::common::SegIDSet segIDSet, OmSegment* desiredRoot);
};


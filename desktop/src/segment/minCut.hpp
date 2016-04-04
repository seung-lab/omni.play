#pragma once
#include "precomp.h"

#include "segment/types.hpp"
#include "segment/boostgraph/types.hpp"
#include "segment/boostgraph/boostGraph.hpp"
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
  MinCut(const OmSegments& segments);

  // i.e. for testing
  MinCut(const OmSegments& segments, const BoostGraphFactory boostGraphFactory);

  // Find the next min cut edge for this source and sink
  om::segment::UserEdge FindEdge(const om::common::SegIDSet sources,
      const om::common::SegIDSet sinks) const;

 private:
  const OmSegments& segments_;
  const BoostGraphFactory boostGraphFactory_;

  bool hasRoot(const om::common::SegIDSet segIDSet, const OmSegment* desiredRoot) const;
};


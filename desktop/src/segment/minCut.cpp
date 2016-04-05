#include "precomp.h"

#include "segment/minCut.hpp"
#include "segment/types.hpp"
#include "segment/boostgraph/types.hpp"
#include "segment/boostgraph/boostGraph.hpp"
#include "common/logging.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/lowLevel/children.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/properties.hpp>

using namespace om::segment::boostgraph;

MinCut::MinCut(const OmSegments& segments)
  : segments_(segments),
    boostGraphFactory_(
        std::make_shared<BoostGraphFactory>(segments.Children())) {}

MinCut::MinCut(const OmSegments& segments, std::shared_ptr<BoostGraphFactory> boostGraphFactory)
  : segments_(segments), boostGraphFactory_(boostGraphFactory) {};

om::segment::UserEdge MinCut::FindEdge(const om::common::SegIDSet sources,
    const om::common::SegIDSet sinks) const {
  if (sources.empty() || sinks.empty()) {
    log_debugs << "Source or sink empty";
    std::cout << "Source or sink empty" << std::endl;
    return om::segment::UserEdge();
  }
  OmSegment* rootSegment = segments_.FindRoot(*sources.begin());

  if (!rootSegment) {
    log_errors << "No root segment found for segID " << *sources.begin();
    std::cout << "No root segment found for segID " << *sources.begin() << std::endl;;
    return om::segment::UserEdge();
  }

  if (!hasRoot(sources, rootSegment)
        || !hasRoot(sinks, rootSegment)) {
    log_debugs << "Source and sink do not share the same root seg " <<
      rootSegment->value();
    std::cout << "Source and sink do not share the same root seg " << rootSegment->value() << std::endl;
    return om::segment::UserEdge();
  }

  std::cout << "get!" << std::endl;
  std::shared_ptr<BoostGraph> boostGraph = boostGraphFactory_->Get(rootSegment);
  std::vector<om::segment::UserEdge> edges = boostGraph->MinCut(sources, sinks);

  if (edges.empty()) {
    log_debugs << "Unable to find a min cut!" << rootSegment->value();
    std::cout << "Unable to find a min cut!" << rootSegment->value() << std::endl;
    return om::segment::UserEdge();
  }

  std::cout << "returning good edge" << std::endl;
  return *edges.begin();
}

bool MinCut::hasRoot(const om::common::SegIDSet segIDSet, const OmSegment* desiredRoot) const {
  for (auto segID : segIDSet) {
    OmSegment* checkingRoot = segments_.FindRoot(segID);
    if (!checkingRoot) {
      log_errors << "No root segment found for segID " << segID << std::endl;
      return false;
    }
    if (checkingRoot != desiredRoot) {
      return false;
    }
  }
  return true;
}

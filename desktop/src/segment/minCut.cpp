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
#include <algorithm>

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
    return om::segment::UserEdge();
  }
  OmSegment* rootSegment = segments_.FindRoot(*sources.begin());

  if (!rootSegment) {
    log_errors << "No root segment found for segID " << *sources.begin();
    return om::segment::UserEdge();
  }

  if (!hasRoot(sources, rootSegment)
        || !hasRoot(sinks, rootSegment)) {
    log_debugs << "Source and sink do not share the same root seg " <<
      rootSegment->value();
    return om::segment::UserEdge();
  }

  std::shared_ptr<BoostGraph> boostGraph = boostGraphFactory_->Get(rootSegment);
  std::vector<om::segment::Edge> edges = boostGraph->MinCut(sources, sinks);

  std::vector<om::segment::UserEdge> userEdges;
  std::transform(edges.begin(), edges.end(), std::back_inserter(userEdges),
      [this](om::segment::Edge edge) {
      return toUserEdge(edge); });
  userEdges.erase(std::remove_if(userEdges.begin(), userEdges.end(),
      [](om::segment::UserEdge userEdge) {
        return !userEdge.valid;
      }), userEdges.end());

  if (userEdges.empty()) {
    log_debugs << "Unable to find a min cut!" << rootSegment->value();
    return om::segment::UserEdge();
  }

  // only return the first for now
  return *userEdges.begin();
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

om::segment::UserEdge MinCut::toUserEdge(om::segment::Edge edge) {
  OmSegment* segment1 = segments_.GetSegment(edge.node1ID);
  OmSegment* segment2 = segments_.GetSegment(edge.node2ID);
  OmSegment* parent, *child;

  om::segment::UserEdge userEdge;
  userEdge.valid = false;

  if (!segment1 || !segment2) {
    return userEdge;
  }


  if (segment1->getParent() && segment1->getParent() == segment2) {
    parent = segment2;
    child = segment1;
  } else if (segment2->getParent() && segment2->getParent() == segment1) {
    parent = segment1;
    child = segment2;
  } else {
    return userEdge;
  }

  userEdge.parentID = parent->value();
  userEdge.childID = child->value();
  userEdge.valid = true;
  return userEdge;
}

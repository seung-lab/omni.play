#pragma once
#include "precomp.h"

#include "segment/minCut.hpp"
#include "segment/types.hpp"
#include "segment/boostgraph/utils.hpp"
#include "segment/boostgraph/types.hpp"
#include "common/logging.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/properties.hpp>

using namespace om::segment::boostgraph;

MinCut::MinCut(OmSegments& segments)
  : segments_(segments), boostGraph_(segments.GetChildren()) {};

MinCut::MinCut(OmSegments& segments, BoostGraph boostGraph)
  : segments_(segments), boostGraph_(boostGraph) {};
    //capacityProperty_(boost::get(boost::edge_capacity, graph_)),
    //colorProperty_(boost::get(boost::vertex_color, graph_)),
    //nameProperty_(boost::get(boost::vertex_name, graph_)),
    //residualCapacityProperty_(boost::get(boost::edge_residual_capacity, graph_)),
    //reverseProperty_(boost::get(boost::edge_reverse, graph_)) {}

om::segment::UserEdge MinCut::FindEdge(om::common::SegIDSet sources,
    om::common::SegIDSet sinks) {
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

  boostGraph.SetRoot(rootSeg);
  std::vector<Edge> edges = boostGraph.minCut(sources, sinks);

  if (edges.empty()) {
    log_debugs << "Unable to find a min cut!" << rootSegment->value();
    std::cout << "Unable to find a min cut!" << rootSegment->value() << std::endl;
    return om::Segment::UserEdge();
  }

  std::cout << "returning good edge" << std::endl;
  return *edges.first();
}

bool MinCut::hasRoot(om::common::SegIDSet segIDSet, OmSegment* desiredRoot) {
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

#pragma once
#include "precomp.h"

#include "segment/types.hpp"
#include "common/logging.h"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/read_dimacs.hpp>
#include <boost/graph/graph_utility.hpp>

class MinCut {

  //typedef adjacency_list_traits< vecS, vecS, directedS> Traits;
  //typedef adjacency_list <vecS, vecS, directedS,
            //property <vertex_name_t, std::string,
            //property <vertex_index_t, long,
            //property <vertex_color_t, boost::default_color_type,
            //property <vertex_distance_t, long,
            //property <vertex_predecessor_t, Traits::edge_descriptor>>>>>,
            //property <edge_capacity_t, long,
            //property <edge_residual_capacity_t, long,
            //property <edge_reverse_t, Traits::edge_descriptor>>>> Graph;

 public:
  MinCut(OmSegments& segments) : segments_(segments) {}

  om::segment::UserEdge findEdge(om::common::SegIDSet sources,
      om::common::SegIDSet sinks) {
    if (sources.empty() || sinks.empty()) {
      log_debugs << "Source or sink empty";
      return om::segment::UserEdge();
    }
    OmSegment* rootSegment = segments_.FindRoot(*sources.begin());

    if (!hasRoot(sources, rootSegment)
          || !hasRoot(sinks, rootSegment)) {
      log_debugs << "Source and sink do not share the same root seg " <<
        rootSegment->value();
      return om::segment::UserEdge();
    }

    return om::segment::UserEdge();
  }

 private:
  OmSegments& segments_;

  bool hasRoot(om::common::SegIDSet segIDSet, OmSegment* desiredRoot) {
    for (auto segID : segIDSet) {
      OmSegment* checkingRoot = segments_.FindRoot(segID);
      if (checkingRoot != desiredRoot) {
        return false;
      }
    }
    return true;
  }
};


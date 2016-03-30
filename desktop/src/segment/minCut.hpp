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

  typedef boost::adjacency_list_traits<boost::vecS, boost::vecS, boost::directedS> Traits;
  typedef typename Traits::vertex_descriptor Vertex;
  typedef typename Traits::edge_descriptor Edge;
  typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::directedS,
            boost::property <boost::vertex_name_t, std::string,
            boost::property <boost::vertex_index_t, long,
            boost::property <boost::vertex_color_t, boost::default_color_type,
            boost::property <boost::vertex_distance_t, long,
            boost::property <boost::vertex_predecessor_t, Edge>>>>>,
            boost::property <boost::edge_capacity_t, long,
            boost::property <boost::edge_residual_capacity_t, long,
            boost::property <boost::edge_reverse_t, Edge>>>> Graph;


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

  Graph createGraph() {
    Graph g;
    boost::property_map<Graph, boost::edge_capacity_t>::type
      capacity = boost::get(boost::edge_capacity, g);
    boost::property_map<Graph, boost::edge_residual_capacity_t>::type
      residual_capacity = boost::get(boost::edge_residual_capacity, g);
    boost::property_map<Graph , boost::edge_reverse_t>::type
      rev = boost::get(boost::edge_reverse, g);

    std::list<Vertex> vertices(10);
    for (int i = 0; i < 10; i ++) {
      vertices.push_back(boost::add_vertex(g));
    }

    Vertex s, t;

    //std::vector<boost::default_color_type> color(boost::num_vertices(g));
    //std::vector<long> distance(boost::num_vertices(g));
    long flow = boost::boykov_kolmogorov_max_flow(g, s, t);

    boost::template graph_traits<Graph>::vertex_iterator u_iter, u_end;
    boost::template graph_traits<Graph>::out_edge_iterator ei, e_end;

    for (boost::tie(u_iter, u_end) = boost::vertices(g); u_iter != u_end; ++u_iter) {
      for (boost::tie(ei, e_end) = boost::out_edges(*u_iter, g); ei != e_end; ++ei) {
        if (capacity[*ei] > 0) {
          std::cout << "f " << *u_iter << " " << boost::target(*ei, g) << " "
            << (capacity[*ei] - residual_capacity[*ei]) << std::endl;
        }
      }
    }
    return g;
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


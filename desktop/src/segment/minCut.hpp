#pragma once
#include "precomp.h"

#include "segment/types.hpp"
#include "common/logging.h"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/boostSegmentGraph.hpp"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/properties.hpp>

using namespace om::segment::boostgraph;

class MinCut {

  Color SOURCE = DefaultColors::black();
  Color SINK = DefaultColors::white();

 public:
  MinCut(OmSegments& segments)
    : segments_(segments), graph_(Graph()),
      capacityProperty_(boost::get(boost::edge_capacity, graph_)),
      colorProperty_(boost::get(boost::vertex_color, graph_)),
      nameProperty_(boost::get(boost::vertex_name, graph_)),
      residualCapacityProperty_(boost::get(boost::edge_residual_capacity, graph_)),
      reverseProperty_(boost::get(boost::edge_reverse, graph_)) {}

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
    om::segment::UserEdge edge;
    edge.valid = true;

    return edge;
  }

  void addEdge(Vertex& vertex1, Vertex& vertex2, int threshold) {
      Edge edgeForward, edgeReverse;
      bool isForwardCreated, isReverseCreated;
      boost::tie(edgeForward, isForwardCreated) =
        boost::add_edge(vertex1, vertex2, graph_);
      boost::tie(edgeReverse, isReverseCreated) =
        boost::add_edge(vertex2, vertex1, graph_);
      if (!isForwardCreated || !isReverseCreated) {
        log_errors << "Unable to create edge correctly between " << vertex1 <<
                  " and " << vertex2 << std::endl;
        return;
      }
      capacityProperty_[edgeForward] = threshold;
      capacityProperty_[edgeReverse] = threshold;
      reverseProperty_[edgeForward] = edgeReverse;
      reverseProperty_[edgeReverse] = edgeForward;
  }

  void populateGraph() {
    std::vector<Vertex> vertices;
    for (int i = 0; i < 10; ++i) {
      Vertex v = boost::add_vertex(graph_);
      vertices.push_back(v);
      boost::put(nameProperty_, v, std::to_string(i));
    }

    for (int i = 1; i < 10; ++i) {
      int cap;
      if (i == 5) {
        cap = 1;
      } else {
        cap = 2;
      }
      addEdge(vertices[i], vertices[i-1], cap);
    }
    Vertex s, t;

    s = *vertices.begin();
    t = *(vertices.end() - 1);
    //std::vector<boost::default_color_type> color(boost::num_vertices(graph_));
    //std::vector<long> distance(boost::num_vertices(graph_));
    std::cout << "running boykov from " << nameProperty_[s] << " to " << nameProperty_[t] << std::endl;
    long flow = boost::boykov_kolmogorov_max_flow(graph_, s, t);
  }

  std::vector<Edge> getMinCutEdges() {
    boost::template graph_traits<Graph>::vertex_iterator u_iter, u_end;
    boost::template graph_traits<Graph>::out_edge_iterator ei, e_end;

    std::vector<Edge> minEdges;
    for (boost::tie(u_iter, u_end) = boost::vertices(graph_); u_iter != u_end; ++u_iter) {
      for (boost::tie(ei, e_end) = boost::out_edges(*u_iter, graph_); ei != e_end; ++ei) {
        Vertex target = boost::target(*ei, graph_);
        if (capacityProperty_[*ei] > 0) {
          std::cout << "f " << *u_iter << " (name = " << nameProperty_[*u_iter] <<
            "color = " << colorProperty_[*u_iter] << ") to " << target <<
            " (name = " << nameProperty_[target] <<
            "color = " << colorProperty_[target] << ") - " <<
            (capacityProperty_[*ei] - residualCapacityProperty_[*ei]) << std::endl;
        }
      }
    }
  }

 private:
  OmSegments& segments_;
  Graph graph_;
  CapacityProperty capacityProperty_;
  ColorProperty colorProperty_;
  NameProperty nameProperty_;
  ResidualCapacityProperty residualCapacityProperty_;
  ReverseProperty reverseProperty_;

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


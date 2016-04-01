#include "precomp.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/properties.hpp>
#include "common/logging.h"
#include <algorithm>

using namespace om::segment::boostgraph;

BoostGraphFactory::BoostGraphFactory() {};
BoostGraphFactory::BoostGraphFactory(const Children& children) 
  : children_(children) {};

std::shared_ptr<BoostGraph> BoostGraphFactory::Get(OmSegment* rootSegment) {
  return std::make_shared<BoostGraph>(children_, rootSegment);
}

BoostGraph::BoostGraph(const Children& children, OmSegment* rootSegment)
  : children_(children), 
    graph_(GetGraph(rootSegment)), idToVertex_(GetVertexMapping(graph_)),
    capacityProperty_(boost::get(boost::edge_capacity, graph_)),
    colorProperty_(boost::get(boost::vertex_color, graph_)),
    nameProperty_(boost::get(boost::vertex_name, graph_)),
    residualCapacityProperty_(boost::get(boost::edge_residual_capacity, graph_)),
    reverseProperty_(boost::get(boost::edge_reverse, graph_)) {}

Graph BoostGraph::GetGraph(OmSegment* rootSeg) {
  return Graph();
}

std::unordered_map<om::common::SegID, Vertex> GetVertexMapping(Graph graph) {
  std::unordered_map<om::common::SegID, Vertex> idToVertex(
      boost::numVertices(graph));
  for (boost::tie(u_iter, u_end) = boost::vertices(graph_);
      u_iter != u_end; ++u_iter) {
    // todo save segment* as graph prop?
  }
}

om::segment::Edge MinCut::toSegmentEdge(Edge edge) {
  om::segment::Edge segmentEdge;
  segmentEdge.valid = true;
  return segmentEdge;
}

std::vector<om::segment::Edge> BoostGraph::MinCut(
    om::common::SegIDSet sources, om::common::SegIDSet sinks) {
  // origin source and sink node
  Vertex vertexS, vertexT;
  if (sources.size() > 1 || sinks.size() > 1) {
    std::tie(vertexS, vertexT) = ConvertToMulti(graph, sources, sinks);
  } else {
    auto iterS = idToVertex.find(*sources.begin());
    auto iterT = idToVertex.find(*sinks.begin());
    if (iterS == sources.end() && iterT == sources.end()) {
      logs_errors << "Unable to find source or sink segments";
      return std::vector<om::common::Edge>();
    }
    vertexS = *iterS;
    vertexT = *iterT;
  }

  boost::boykov_kolmogorov_max_flow(graph_, vertexS, vertexT);

  std::vector<Edge> edges = GetMinCutEdges();
  return std::transform(edges.begin(), edges().end(), ToSegmentEdge);
}

void MinCut::populateGraph() {
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
}

std::vector<Edge> MinCut::getMinCutEdges(Graph graph) {
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

Graph toGraphFromRootSeg(OmSegment* rootSegment, OmSegments& segments,
      Children& children) {

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
              " and " << vertex2;
    std::cout << "Unable to create edge correctly between " << vertex1 <<
              " and " << vertex2 << std::endl;
    return;
  }
  capacityProperty_[edgeForward] = threshold;
  capacityProperty_[edgeReverse] = threshold;
  reverseProperty_[edgeForward] = edgeReverse;
  reverseProperty_[edgeReverse] = edgeForward;
}

} //namespace boostgraph
} //namespace segment
}

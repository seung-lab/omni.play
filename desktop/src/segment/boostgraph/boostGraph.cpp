#include "precomp.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/properties.hpp>
#include "segment/lowLevel/children.hpp"
#include "segment/boostgraph/types.hpp"
#include "segment/boostgraph/boostGraph.hpp"
#include "common/logging.h"
#include <algorithm>

using namespace om::segment::boostgraph;
BoostGraph::BoostGraph(const om::segment::Children& children) 
  : children_(children) {}

BoostGraph::BoostGraph(const om::segment::Children& children, const OmSegment* rootSegment)
  : children_(children),
    nameProperty_(boost::get(boost::vertex_name, graph_)),
    capacityProperty_(boost::get(boost::edge_capacity, graph_)),
    reverseProperty_(boost::get(boost::edge_reverse, graph_)),
    colorProperty_(boost::get(boost::vertex_color, graph_)) {
      BuildGraph(rootSegment);
    }
    //residualCapacityProperty_(boost::get(boost::edge_residual_capacity, graph_)),

Graph BoostGraph::GetGraph() { return graph_; }
void BoostGraph::SetGraph(Graph graph) { graph_ = graph; }

std::vector<om::segment::UserEdge> BoostGraph::MinCut(
    const om::common::SegIDSet sources, const om::common::SegIDSet sinks) {
  // source and sink vertices
  Vertex vertexS, vertexT;
  if (sources.size() > 1 || sinks.size() > 1) {
    std::tie(vertexS, vertexT) = MakeSingleSourceSink(sources, sinks);
  } else {
    auto iterS = idToVertex_.find(*sources.begin());
    auto iterT = idToVertex_.find(*sinks.begin());
    if (iterS == idToVertex_.end() && iterT == idToVertex_.end()) {
      log_errors << "Unable to find source or sink segments";
      return std::vector<om::segment::UserEdge>();
    }
    vertexS = iterS->second;
    vertexT = iterT->second;
  }

  boost::boykov_kolmogorov_max_flow(graph_, vertexS, vertexT);

  std::vector<Edge> edges = GetMinCutEdges();
  std::vector<om::segment::UserEdge> userEdges;
  std::transform(edges.begin(), edges.end(), std::back_inserter(userEdges),
      [](Edge edge) { return ToSegmentUserEdge(edge); });
  return userEdges;
}

std::tuple<Vertex, Vertex> BoostGraph::MakeSingleSourceSink(
    const om::common::SegIDSet sources, const om::common::SegIDSet sinks) {
  return std::tuple<Vertex, Vertex>(Vertex(), Vertex());
}
// TODO
std::vector<Edge> BoostGraph::GetMinCutEdges() {
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
          "color = " << colorProperty_[target] << ") - ";
        //<< (capacityProperty_[*ei] - residualCapacityProperty_[*ei]) << std::endl;
      }
    }
  }
  return minEdges;
}

// TODO 
om::segment::UserEdge BoostGraph::ToSegmentUserEdge(const Edge edge) {
  om::segment::UserEdge segmentEdge;
  segmentEdge.valid = true;
  return segmentEdge;
}

Graph BoostGraph::BuildGraph(const OmSegment* rootSegment) {
  graph_.clear();

  // These are the only properties we are interested
  nameProperty_ = boost::get(boost::vertex_name, graph_);
  capacityProperty_ = boost::get(boost::edge_capacity, graph_);
  reverseProperty_ = boost::get(boost::edge_reverse, graph_);

  std::cout << "Buildg raph now" << std::endl;
  Vertex rootVertex = addVertex(rootSegment);
  buildGraphDfsVisit(rootSegment);
  return graph_;
}

void BoostGraph::buildGraphDfsVisit(const OmSegment* parent) {
  std::cout << "Buildg raph now" << parent->value() << std::endl;
  for (const OmSegment* child : children_.GetChildren(parent->value())) {
  std::cout << "Buildg raph now" << child->value() << std::endl;
    Vertex childVertex = addVertex(child);
    auto parentVertexIter = idToVertex_.find(parent->value());
    if (parentVertexIter != idToVertex_.end()) {
      addEdge(parentVertexIter->second, childVertex, child->getThreshold());
    } else {
      log_errors << "Should have already created vertex for " << parent->value() <<
        " but it was not found";
      return;
    }
    buildGraphDfsVisit(child);
  }
}

// Uses graph_, idToVertex_, nameProperty_
Vertex BoostGraph::addVertex(const OmSegment* segment) {
  Vertex vertex = boost::add_vertex(graph_);
  idToVertex_.insert({segment->value(), vertex});
  boost::put(nameProperty_, vertex, std::to_string(segment->value()));
  return vertex;
}

// Uses graph_, capacityProperty_ and reverseProperty_
void BoostGraph::addEdge(Vertex& vertex1, Vertex& vertex2, int threshold) {
  Edge edgeForward, edgeReverse;
  bool isForwardCreated, isReverseCreated;
  boost::tie(edgeForward, isForwardCreated) =
    boost::add_edge(vertex1, vertex2, graph_);
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

BoostGraphFactory::BoostGraphFactory(const om::segment::Children& children) 
  : children_(children) {
    std::cout << "BG Base done" << std::endl;};

std::shared_ptr<BoostGraph> BoostGraphFactory::Get(const OmSegment* rootSegment) const {
  return std::make_shared<BoostGraph>(children_, rootSegment);
}


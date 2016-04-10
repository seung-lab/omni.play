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
const double BoostGraph::HARD_LINK_WEIGHT = 100;
const Color BoostGraph::COLOR_SOURCE = DefaultColors::black();
const Color BoostGraph::COLOR_SINK = DefaultColors::white();

BoostGraph::BoostGraph(const om::segment::Children& children) 
  : children_(children) {}

BoostGraph::BoostGraph(const om::segment::Children& children, const OmSegment* rootSegment)
  : children_(children),
    nameProperty_(boost::get(boost::vertex_name, graph_)),
    capacityProperty_(boost::get(boost::edge_capacity, graph_)),
    reverseProperty_(boost::get(boost::edge_reverse, graph_)),
    colorProperty_(boost::get(boost::vertex_color, graph_)),
    segmentIDProperty_(boost::get(vertex_segmentID(), graph_)) {
      BuildGraph(rootSegment);
    }

void BoostGraph::SetGraph(Graph graph) { graph_ = graph; setProperties(); }
Graph& BoostGraph::GetGraph() { return graph_; }
Vertex& BoostGraph::GetVertex(om::common::SegID segID) { return idToVertex_[segID]; }

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
  // create a new common source and connect it to the desired sources
  Vertex commonSource = boost::add_vertex(graph_);
  nameProperty_[commonSource] = "Source";
  for (auto id : sources) {
    auto userSourceVertexIter = idToVertex_.find(id);
    if (userSourceVertexIter != idToVertex_.end()) {
      addEdge(commonSource, userSourceVertexIter->second, HARD_LINK_WEIGHT);

      // disallow reverse flow from this edge!
      Edge edge;
      bool edgeIsFound;
      std::tie(edge, edgeIsFound) =
        boost::edge(userSourceVertexIter->second, commonSource, graph_);
      capacityProperty_[edge] = 0;
    }
  }

  // create a new common sink and connect it to the desired sinks
  Vertex commonSink = boost::add_vertex(graph_);
  nameProperty_[commonSink] = "Sink";
  for (auto id : sinks) {
    auto userSinkVertexIter = idToVertex_.find(id);
    if (userSinkVertexIter != idToVertex_.end()) {
      addEdge(userSinkVertexIter->second, commonSink,
          BoostGraph::HARD_LINK_WEIGHT);

      // disallow reverse flow from this edge!
      Edge edge;
      bool edgeIsFound;
      std::tie(edge, edgeIsFound) =
        boost::edge(commonSink, userSinkVertexIter->second, graph_);
      capacityProperty_[edge] = 0;
    }
  }

  return std::tuple<Vertex, Vertex>(commonSource, commonSink);
}
// TODO
std::vector<Edge> BoostGraph::GetMinCutEdges(Vertex sourceVertex) {
  boost::template graph_traits<Graph>::vertex_iterator vIter, vIterEnd;
  boost::template graph_traits<Graph>::out_edge_iterator eIter, eIterEnd;

  //std::unordered_set<vertex>
  //std::queue<Vertex> queue

  std::vector<Edge> minEdges;
  return minEdges;
}

om::segment::UserEdge BoostGraph::ToSegmentUserEdge(const Edge edge) {
  om::segment::UserEdge segmentEdge;
  segmentEdge.parentID = segmentIDProperty_[boost::source(edge, graph_)];
  segmentEdge.childID = segmentIDProperty_[boost::target(edge, graph_)];
  segmentEdge.valid = true;
  return segmentEdge;
}

void BoostGraph::BuildGraph(const OmSegment* rootSegment) {
  graph_.clear();

  setProperties();
  std::cout << "Buid Graph adding vertex " << rootSegment->value() << std::endl;
  Vertex rootVertex = addVertex(rootSegment);
  buildGraphDfsVisit(rootVertex);
}

void BoostGraph::setProperties() {
  // These are the only properties we are interested
  nameProperty_ = boost::get(boost::vertex_name, graph_);
  capacityProperty_ = boost::get(boost::edge_capacity, graph_);
  reverseProperty_ = boost::get(boost::edge_reverse, graph_);
  colorProperty_ = boost::get(boost::vertex_color, graph_);
  segmentIDProperty_ = boost::get(vertex_segmentID(), graph_);
}

void BoostGraph::buildGraphDfsVisit(Vertex parentVertex) {
  om::common::SegID parentSegmentID = segmentIDProperty_[parentVertex];
  std::cout << "buildGraphdfsvisit " << parentSegmentID << std::endl;

  for (const OmSegment* child : children_.GetChildren(parentSegmentID)) {
    std::cout << "parent " << parentSegmentID << " has child " << 
      child->value() << std::endl;
    Vertex childVertex = addVertex(child);
    auto parentVertexIter = idToVertex_.find(parentSegmentID);
    if (parentVertexIter != idToVertex_.end()) {
      addEdge(parentVertexIter->second, childVertex, child->getThreshold());
    } else {
      log_errors << "Should have already created vertex for " << parentSegmentID <<
        " but it was not found";
      return;
    }
    buildGraphDfsVisit(childVertex);
  }
}

// Uses graph_, idToVertex_, nameProperty_
Vertex BoostGraph::addVertex(const OmSegment* segment) {
  Vertex vertex = boost::add_vertex(graph_);
  idToVertex_.insert({segment->value(), vertex});
  nameProperty_[vertex] = std::to_string(segment->value());
  segmentIDProperty_[vertex] = segment->value();
  return vertex;
}

// Uses graph_, capacityProperty_ and reverseProperty_
void BoostGraph::addEdge(Vertex& vertex1, Vertex& vertex2, double threshold) {
  Edge edgeForward, edgeReverse;
  bool isForwardCreated, isReverseCreated;
  boost::tie(edgeForward, isForwardCreated) =
    boost::add_edge(vertex1, vertex2, graph_);
  boost::tie(edgeReverse, isReverseCreated) =
    boost::add_edge(vertex2, vertex1, graph_);
  if (!isForwardCreated || !isReverseCreated) {
    log_errors << "Unable to create edge correctly between " << vertex1 <<
              " and " << vertex2 << " created forward " << 
              isForwardCreated << " created reverse " << isReverseCreated <<
              std::endl;
    std::cout << "Unable to create edge correctly between " << vertex1 <<
              " and " << vertex2 << " created forward " << 
              isForwardCreated << " created reverese " << isReverseCreated <<
              std::endl;
    return;
  } else {
    std::cout << " Added edge from " << segmentIDProperty_[vertex1] << " to " << segmentIDProperty_[vertex2] << 
  "with threshold " << threshold << std::endl;
  }
  capacityProperty_[edgeForward] = threshold;
  capacityProperty_[edgeReverse] = threshold;
  reverseProperty_[edgeForward] = edgeReverse;
  reverseProperty_[edgeReverse] = edgeForward;
}

BoostGraphFactory::BoostGraphFactory(const om::segment::Children& children) 
  : children_(children) {};

std::shared_ptr<BoostGraph> BoostGraphFactory::Get(const OmSegment* rootSegment) const {
  return std::make_shared<BoostGraph>(children_, rootSegment);
}


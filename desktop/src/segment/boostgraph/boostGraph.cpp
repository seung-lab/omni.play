#include "precomp.h"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/boykov_kolmogorov_max_flow.hpp>
#include <boost/graph/properties.hpp>
#include "segment/lowLevel/children.hpp"
#include "segment/boostgraph/types.hpp"
#include "segment/boostgraph/boostGraph.hpp"
#include "common/logging.h"
#include <algorithm>
#include <sstream>

using namespace om::segment::boostgraph;
const double BoostGraph::HARD_LINK_WEIGHT = 100;
const Color BoostGraph::COLOR_SOURCE = DefaultColors::black();
const Color BoostGraph::COLOR_SINK = DefaultColors::white();
const Color BoostGraph::COLOR_FREE = DefaultColors::gray();

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

std::vector<om::segment::Edge> BoostGraph::MinCut(
    const om::common::SegIDSet sources, const om::common::SegIDSet sinks) {
  std::stringstream cutInfoStream;
  cutInfoStream << "Searching for cut between (";
  for (auto id : sources) {
    cutInfoStream << id << ", ";
  }
  cutInfoStream << ") to (";
  for (auto id : sinks) {
    cutInfoStream << id << ", ";
  }
  cutInfoStream << ")";
  std::cout << cutInfoStream.str() << std::endl;;

  // source and sink vertices
  Vertex vertexS, vertexT;
  std::tie(vertexS, vertexT) = MakeSingleSourceSink(sources, sinks);

  boost::boykov_kolmogorov_max_flow(graph_, vertexS, vertexT);

  std::vector<Edge> edges = GetMinCutEdges(vertexS);
  std::vector<om::segment::Edge> segmentEdges;
  std::transform(edges.begin(), edges.end(), std::back_inserter(segmentEdges),
      [this](Edge edge) { return ToSegmentEdge(edge); });
  std::stringstream returnCutEdgesStream;
  returnCutEdgesStream << "trying to cut ";
  for (auto edge : segmentEdges) {
    returnCutEdgesStream << "(" << edge.node1ID << "," << edge.node2ID << ")";
  }
  std::cout << returnCutEdgesStream.str() << std::endl;;
  return segmentEdges;
}

std::tuple<Vertex, Vertex> BoostGraph::MakeSingleSourceSink(
    const om::common::SegIDSet sources, const om::common::SegIDSet sinks) {
  Vertex commonSource = createCommonVertex(sources, true);
  Vertex commonSink = createCommonVertex(sinks, false);

  return std::tuple<Vertex, Vertex>(commonSource, commonSink);
}

std::vector<Edge> BoostGraph::GetMinCutEdges(Vertex sourceVertex) {
  std::queue<Vertex> queue;
  std::unordered_set<Vertex> visited;

  std::vector<Edge> minEdges;

  queue.push(sourceVertex);

  while(!queue.empty()) {
    Vertex sourceVertex = queue.front();
    visited.insert(sourceVertex);

    boost::template graph_traits<Graph>::out_edge_iterator eIter, eIterEnd;
    for(boost::tie(eIter, eIterEnd) = boost::out_edges(sourceVertex, graph_);
        eIter != eIterEnd; ++eIter) {
      Vertex targetVertex = boost::target(*eIter, graph_);

      // we found a source --> sink edge, add this to the list
      // also we only add to the list if it has a valid segmentID (non-zero)
      if (colorProperty_[targetVertex] == COLOR_SINK
          || colorProperty_[targetVertex] == COLOR_FREE
          && segmentIDProperty_[sourceVertex]
          && segmentIDProperty_[targetVertex]) {
        minEdges.push_back(*eIter);
        continue;
      }

      // only add the target vertex if it is a source and we haven't visited yet
      if (colorProperty_[targetVertex] == COLOR_SOURCE
         && visited.find(targetVertex) == visited.end()) {
        queue.push(targetVertex);
      }
    }
    queue.pop();
  }

  return minEdges;
}

om::segment::Edge BoostGraph::ToSegmentEdge(const Edge edge) {
  om::segment::Edge segmentEdge;
  segmentEdge.node1ID = segmentIDProperty_[boost::source(edge, graph_)];
  segmentEdge.node2ID = segmentIDProperty_[boost::target(edge, graph_)];
  segmentEdge.threshold = capacityProperty_[edge];
  return segmentEdge;
}

void BoostGraph::BuildGraph(const OmSegment* rootSegment) {
  graph_.clear();

  if (!rootSegment) {
    return;
  }

  setProperties();
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

  for (const OmSegment* child : children_.GetChildren(parentSegmentID)) {
    if (!child) {
      continue;
    }
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
std::tuple<Edge, Edge, bool> BoostGraph::addEdge(Vertex& vertex1,
    Vertex& vertex2, double threshold) {
  Edge edgeForward, edgeReverse;
  bool isForwardCreated, isReverseCreated, isSuccess = true;
  boost::tie(edgeForward, isForwardCreated) =
    boost::add_edge(vertex1, vertex2, graph_);
  boost::tie(edgeReverse, isReverseCreated) =
    boost::add_edge(vertex2, vertex1, graph_);
  if (!isForwardCreated || !isReverseCreated) {
    log_errors << "Unable to create edge correctly between " << vertex1 <<
              " and " << vertex2 << " created forward " << 
              isForwardCreated << " created reverse " << isReverseCreated;
    isSuccess = false;
  } else {
    capacityProperty_[edgeForward] = threshold;
    capacityProperty_[edgeReverse] = threshold;
    reverseProperty_[edgeForward] = edgeReverse;
    reverseProperty_[edgeReverse] = edgeForward;
  }
  return std::make_tuple(edgeForward, edgeReverse, isSuccess);
}

Vertex BoostGraph::createCommonVertex(om::common::SegIDSet ids,
    bool isSource) {
  Vertex commonVertex = boost::add_vertex(graph_);
  nameProperty_[commonVertex] = isSource ? "Source" : "Sink";
  for (auto id : ids) {
    auto connectingVertexIter = idToVertex_.find(id);
    if (connectingVertexIter != idToVertex_.end()) {
      Vertex connectingVertex = connectingVertexIter->second;

      Edge forwardEdge, reverseEdge;
      bool isEdgesCreated;
      std::tie(forwardEdge, reverseEdge, isEdgesCreated) =
        addEdge(commonVertex, connectingVertex, HARD_LINK_WEIGHT);

      if (isEdgesCreated) {
        // disallow reverse flow from this edge!
        capacityProperty_[isSource ? reverseEdge : forwardEdge] = 0;
      } else {
        log_errors << "Unable to create common vertex edge to " <<
          id;
      }
    } else {
      log_errors << "Unable to find vertex with id " << id <<
        " (Generated mincut graph does not match OmSegment structure!";
    }
  }
  return commonVertex;
}

BoostGraphFactory::BoostGraphFactory(const om::segment::Children& children) 
  : children_(children) {};

std::shared_ptr<BoostGraph> BoostGraphFactory::Get(const OmSegment* rootSegment) const {
  return std::make_shared<BoostGraph>(children_, rootSegment);
}


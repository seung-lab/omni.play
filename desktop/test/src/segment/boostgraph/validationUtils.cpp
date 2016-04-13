#pragma once
#include "validationUtils.hpp"
#include "precomp.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "segment/omSegment.h"
#include "segment/boostgraph/boostGraph.hpp"
#include "segment/boostgraph/types.hpp"

namespace test {
namespace segment {
namespace boostgraph {

void validateSegmentBuilt(OmSegment* segment, Graph graph) {
  OmSegment* parentSegment = segment->getParent();
  SegmentIDProperty segmentIDProperty = boost::get(vertex_segmentID(), graph);
  CapacityProperty capacityProperty = boost::get(boost::edge_capacity, graph);

  bool segmentIsFound;
  bool parentIsFound;
  Vertex segmentVertex = 0;
  Vertex parentVertex = 0;

  boost::template graph_traits<Graph>::vertex_iterator u_iter, u_end;
  for (boost::tie(u_iter, u_end) = boost::vertices(graph);
      u_iter != u_end; ++u_iter) {
    om::common::SegID vertexSegmentID = segmentIDProperty[*u_iter];
    if (vertexSegmentID == segment->value()) {
      segmentIsFound = true;
      segmentVertex = *u_iter;
    } else if (parentSegment && vertexSegmentID == parentSegment->value()) {
        parentIsFound = true;
        parentVertex = *u_iter;
    }
  }
  ASSERT_TRUE(segmentIsFound);
  if (parentSegment) {
    ASSERT_TRUE(parentIsFound) << "Parent " << parentSegment->value() <<
      " was not found for child " << segment->value();

    Edge edge;
    bool edgeIsFound;
    std::tie(edge, edgeIsFound) =
      boost::edge(parentVertex, segmentVertex, graph);
    ASSERT_TRUE(edgeIsFound);
    ASSERT_EQ(segment->getThreshold(), capacityProperty[edge]);
  }
}

void verifySinkSource(Vertex newSourceVertex, Vertex newSinkVertex,
    om::common::SegIDSet sources, om::common::SegIDSet sinks,
    BoostGraph boostGraph) {
  Graph graph = boostGraph.GetGraph();
  SegmentIDProperty segmentIDProperty = boost::get(vertex_segmentID(), graph);
  CapacityProperty capacityProperty = boost::get(boost::edge_capacity, graph);

  Edge edge;
  bool edgeIsFound;

  EXPECT_EQ(sources.size(), boost::out_degree(newSourceVertex, graph));
  for (auto id : sources) {
    Vertex connectedSource = boostGraph.GetVertex(id);
    std::tie(edge, edgeIsFound) =
      boost::edge(newSourceVertex, connectedSource, graph);
    EXPECT_TRUE(edgeIsFound);
    if (edgeIsFound) {
      EXPECT_EQ(BoostGraph::HARD_LINK_WEIGHT, capacityProperty[edge]);
    }
    // verify reverse edge exists but capacity is 0
    std::tie(edge, edgeIsFound) =
      boost::edge(connectedSource, newSourceVertex, graph);
    EXPECT_TRUE(edgeIsFound);
    if (edgeIsFound) {
      EXPECT_EQ(0, capacityProperty[edge]);
    }
  }

  // Because this is technically a directional graph (not bidirectional)
  // boost does not provide in_degree
  boost::template graph_traits<Graph>::vertex_iterator vIter, vIterEnd;
  for (boost::tie(vIter, vIterEnd) = boost::vertices(graph);
      vIter != vIterEnd; ++vIter) {
    auto segmentID = segmentIDProperty[*vIter];

    // since we're iterating through all the vertices we only need to test
    // if the vertex is one of the sinks
    bool isDesiredSink = sinks.find(segmentID) != sinks.end();
    if (isDesiredSink) {
      std::tie(edge, edgeIsFound) =
        boost::edge(*vIter, newSinkVertex, graph);
      EXPECT_TRUE(edgeIsFound);
      if (edgeIsFound) {
        EXPECT_EQ(BoostGraph::HARD_LINK_WEIGHT, capacityProperty[edge]);
      }
      // verify reverse edge has capcity of 0
      std::tie(edge, edgeIsFound) =
        boost::edge(newSinkVertex, *vIter, graph);
      EXPECT_TRUE(edgeIsFound);
      if (edgeIsFound) {
        EXPECT_EQ(0, capacityProperty[edge]);
      }
    }
  }
}

template <typename U_EXP, typename V_ACT>
void validateCutEdges(std::vector<std::tuple<U_EXP, U_EXP>>& expectedEdges,
    std::vector<V_ACT>& actualEdges, Graph& graph,
    std::function<U_EXP(V_ACT)> edgeToSourceFunction,
    std::function<U_EXP(V_ACT)> edgeToTargetFunction) {
  expectedEdges.erase(std::remove_if(expectedEdges.begin(), expectedEdges.end(),
        [&actualEdges, &graph, &edgeToSourceFunction, &edgeToTargetFunction]
        (std::tuple<U_EXP, U_EXP> vertexTuple) {
          return std::count_if(actualEdges.begin(), actualEdges.end(),
            [&vertexTuple, &graph, &edgeToSourceFunction,
            &edgeToTargetFunction](V_ACT actualEdge) {
              U_EXP edgeSource, edgeTarget;
              std::tie(edgeSource, edgeTarget) = vertexTuple;
              U_EXP cutSource = edgeToSourceFunction(actualEdge);
              U_EXP cutTarget = edgeToTargetFunction(actualEdge);
              // in this case we're just cutting so order doesn't matter
              return (edgeSource == cutSource && edgeTarget == cutTarget)
                || (edgeSource == cutTarget && edgeTarget == cutSource);
            }) > 0;
        }), expectedEdges.end());
  EXPECT_EQ(0, expectedEdges.size()) << " Cut edges returned did not include" <<
    " all expected edges";
  if (expectedEdges.size() > 0) {
    std::cerr << "Expected edge that wasn't returned ";
    for (auto edge : expectedEdges) {
      std::cerr << "(" << std::get<0>(edge) << ", " << std::get<1>(edge) << "), ";
    }
    std::cerr << std::endl;
  }
}

std::tuple<std::function<Vertex(Edge)>, std::function<Vertex(Edge)>>
  getEdgeToVertexFunctions(Graph& graph) {
  return std::make_tuple(
      [&graph](Edge edge) { return boost::source(edge, graph); },
      [&graph](Edge edge) { return boost::target(edge, graph); });
}

std::tuple<std::function<om::common::SegID(om::segment::UserEdge)>,
  std::function<om::common::SegID(om::segment::UserEdge)>>
  getUserEdgeToSegIDFunctions() {
  std::function<om::common::SegID(om::segment::UserEdge)> sourceFunction =
    [](om::segment::UserEdge userEdge) { return userEdge.parentID; };
  std::function<om::common::SegID(om::segment::UserEdge)> targetFunction =
    [](om::segment::UserEdge userEdge) { return userEdge.childID; };
  return std::make_tuple(sourceFunction, targetFunction);
}

} // namespace boostgraph
} // namespace segment
} // namespace test


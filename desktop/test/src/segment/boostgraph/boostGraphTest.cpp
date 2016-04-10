#pragma once
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "segment/omSegment.h"
#include "segment/lowLevel/children.hpp"
#include "segment/boostgraph/types.hpp"
#include "segment/boostgraph/boostGraph.hpp"
#include "segment/testSetup.hpp"

using namespace test::segment;

namespace test {
namespace boostGraph {

/*
 * Makes sure that the given element exists and it's connect to the correct
 * parent with the correct weight
 */
void validateSegment(OmSegment* segment, Graph graph) {
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

std::vector<std::unique_ptr<OmSegment>> getBasicLineGraph(uint32_t numSegments,
    std::vector<om::segment::Data>& data, MockChildren& mockChildren,
    ChildrenList& childrenList) {

  std::vector<std::unique_ptr<OmSegment>> segments;
  segments.push_back(createSegment(1, data, mockChildren, childrenList));

  for (uint32_t index = 1; index < numSegments; ++index) {
    // since segid 0 does not exist, the segid is essentially 1 + index
    om::common::SegID segmentID = index + 1;
    segments.push_back(createSegment(segmentID, data, mockChildren, childrenList));

    addToChildren(segments[index - 1].get(),
        segments[index].get(), segments[index - 1]->value()/10,
        mockChildren);
  }
  return segments;
}

TEST(boostGraph, testBasicGraph) {
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  uint32_t numSegments = 4;
  std::tie(data, childrenList) = prepareSegmentData(numSegments);

  std::vector<std::unique_ptr<OmSegment>> segments =
    getBasicLineGraph(numSegments, data, mockChildren, childrenList);

  boostGraph.BuildGraph(segments[0].get());
  Graph& graph = boostGraph.GetGraph();

  for (auto& segment : segments) {
    validateSegment(segment.get(), graph);
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
    EXPECT_EQ(BoostGraph::HARD_LINK_WEIGHT, capacityProperty[edge]);
  }

  // Because this is technically a directional graph (not bidirectional)
  // boost does not provide in_degree
  boost::template graph_traits<Graph>::vertex_iterator vIter, vIterEnd;
  for (boost::tie(vIter, vIterEnd) = boost::vertices(graph);
      vIter != vIterEnd; ++vIter) {
    auto segmentID = segmentIDProperty[*vIter];
    std::tie(edge, edgeIsFound) =
      boost::edge(*vIter, newSinkVertex, graph);

    bool isDesiredSink = sinks.find(segmentID) != sinks.end();
    EXPECT_TRUE(isDesiredSink == edgeIsFound);
    if (edgeIsFound) {
      EXPECT_EQ(BoostGraph::HARD_LINK_WEIGHT, capacityProperty[edge]);
    }
  }
}

TEST(boostGraph, testSingleSourceSinkSingle) {
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  uint32_t numSegments = 4;
  std::tie(data, childrenList) = prepareSegmentData(numSegments);

  std::vector<std::unique_ptr<OmSegment>> segments =
    getBasicLineGraph(numSegments, data, mockChildren, childrenList);

  boostGraph.BuildGraph(segments[0].get());
  Graph& graph = boostGraph.GetGraph();

  om::common::SegIDSet sources;
  sources.insert(segments[0]->value());
  om::common::SegIDSet sinks;
  sinks.insert(segments[segments.size() - 1]->value());

  Vertex sourceVertex, sinkVertex;
  std::tie(sourceVertex, sinkVertex) =
    boostGraph.MakeSingleSourceSink(sources, sinks);

  verifySinkSource(sourceVertex, sinkVertex, sources, sinks,
      boostGraph);
}

TEST(boostGraph, testMultieSourceSinkSingle) {
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  uint32_t numSegments = 4;
  std::tie(data, childrenList) = prepareSegmentData(numSegments);

  std::vector<std::unique_ptr<OmSegment>> segments =
    getBasicLineGraph(numSegments, data, mockChildren, childrenList);

  boostGraph.BuildGraph(segments[0].get());
  Graph& graph = boostGraph.GetGraph();

  om::common::SegIDSet sources;
  sources.insert(segments[0]->value());
  sources.insert(segments[1]->value());
  om::common::SegIDSet sinks;
  sinks.insert(segments[segments.size() - 1]->value());
  sinks.insert(segments[segments.size() - 2]->value());

  Vertex sourceVertex, sinkVertex;
  std::tie(sourceVertex, sinkVertex) =
    boostGraph.MakeSingleSourceSink(sources, sinks);

  verifySinkSource(sourceVertex, sinkVertex, sources, sinks,
      boostGraph);
}

TEST(boostGraph, testFindMinCutEdgesBasic) {
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  uint32_t numSegments = 4;
  std::tie(data, childrenList) = prepareSegmentData(numSegments);

  std::vector<std::unique_ptr<OmSegment>> segments =
    getBasicLineGraph(numSegments, data, mockChildren, childrenList);

  boostGraph.BuildGraph(segments[0].get());
  Graph& graph = boostGraph.GetGraph();

  // set the edge between seg 2 and 3 to be the min cut edge
  ColorProperty colorProperty = boost::get(boost::vertex_color, graph);
  SegmentIDProperty segmentIDProperty = boost::get(vertex_segmentID(), graph);
  uint32_t pivot = 2;
  for (auto& segment : segments) {
    Vertex vertex = boostGraph.GetVertex(segment->value());
    colorProperty[vertex] = segment->value() <= pivot ? 
      BoostGraph::COLOR_SOURCE : BoostGraph::COLOR_SINK;
  }

  std::vector<Edge> cutEdges = boostGraph.GetMinCutEdges(
      boostGraph.GetVertex(segments[0]->value()));

  // check to make sure the cut edge is between seg 2 and 3
  ASSERT_EQ(1, cutEdges.size());
  Edge cutEdge = cutEdges[0];
  Vertex source = boost::source(cutEdge, graph);
  Vertex target = boost::target(cutEdge, graph);
  EXPECT_EQ(segments[1]->value(), segmentIDProperty[source]);
  EXPECT_EQ(segments[2]->value(), segmentIDProperty[target]);
}

} //namespace boostgraph
} //namespace test

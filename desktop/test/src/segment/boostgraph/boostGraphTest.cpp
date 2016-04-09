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
    std::tie(edge, edgeIsFound) = boost::edge(parentVertex, segmentVertex, graph);
    ASSERT_TRUE(edgeIsFound);
    ASSERT_EQ(segment->getThreshold(), capacityProperty[edge]);
  }
}

TEST(boostGraph, testBasicGraph) {
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  std::tie(data, childrenList) = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(1, data,
      mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2 = createSegment(2, data,
      mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2_3 = createSegment(3, data,
      mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2_3_4 = createSegment(4, data,
      mockChildren, childrenList);

  addToChildren(segment1.get(), segment1_2.get(), segment1->value()/10., mockChildren);
  addToChildren(segment1_2.get(), segment1_2_3.get(), segment1_2->value()/10., mockChildren);
  addToChildren(segment1_2_3.get(), segment1_2_3_4.get(), segment1_2_3->value()/10., mockChildren);

  boostGraph.BuildGraph(segment1.get());
  Graph& graph = boostGraph.GetGraph();

  validateSegment(segment1.get(), graph);
  validateSegment(segment1_2.get(), graph);
  validateSegment(segment1_2_3.get(), graph);
  validateSegment(segment1_2_3_4.get(), graph);
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
  //EXPECT_EQ(sources.size(), outDegrees);
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
    EXPECT_EQ(BoostGraph::HARD_LINK_WEIGHT, capacityProperty[edge]);
  }
}

TEST(boostGraph, testSingleSourceSinkSingle) {
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  std::tie(data, childrenList) = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(1, data,
      mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2 = createSegment(2, data,
      mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2_3 = createSegment(3, data,
      mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2_3_4 = createSegment(4, data,
      mockChildren, childrenList);

  addToChildren(segment1.get(), segment1_2.get(), segment1->value()/10., mockChildren);
  addToChildren(segment1_2.get(), segment1_2_3.get(), segment1_2->value()/10., mockChildren);
  addToChildren(segment1_2_3.get(), segment1_2_3_4.get(), segment1_2_3->value()/10., mockChildren);

  boostGraph.BuildGraph(segment1.get());

  om::common::SegIDSet sources;
  sources.insert(segment1->value());
  om::common::SegIDSet sinks;
  sinks.insert(segment1_2_3_4->value());

  Vertex sourceVertex, sinkVertex;
  std::tie(sourceVertex, sinkVertex) =
    boostGraph.MakeSingleSourceSink(sources, sinks);

  verifySinkSource(sourceVertex, sinkVertex, sources, sinks,
      boostGraph);
  }

TEST(boostGraph, testMultiSourceSink) {
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  std::tie(data, childrenList) = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(1, data,
      mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2 = createSegment(2, data,
      mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2_3 = createSegment(3, data,
      mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2_3_4 = createSegment(4, data,
      mockChildren, childrenList);

  addToChildren(segment1.get(), segment1_2.get(), segment1->value()/10., mockChildren);
  addToChildren(segment1_2.get(), segment1_2_3.get(), segment1_2->value()/10., mockChildren);
  addToChildren(segment1_2_3.get(), segment1_2_3_4.get(), segment1_2_3->value()/10., mockChildren);

  boostGraph.BuildGraph(segment1.get());

  om::common::SegIDSet sources;
  sources.insert(segment1->value());
  sources.insert(segment1_2->value());
  om::common::SegIDSet sinks;
  sinks.insert(segment1_2_3->value());
  sinks.insert(segment1_2_3_4->value());

  Vertex sourceVertex, sinkVertex;
  std::tie(sourceVertex, sinkVertex) =
    boostGraph.MakeSingleSourceSink(sources, sinks);

  verifySinkSource(sourceVertex, sinkVertex, sources, sinks,
      boostGraph);
}
} //namespace boostgraph
} //namespace test

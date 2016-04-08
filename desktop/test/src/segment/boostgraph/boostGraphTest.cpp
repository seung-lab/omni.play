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
  Vertex segmentVertex;
  Vertex parentVertex;

  boost::template graph_traits<Graph>::vertex_iterator u_iter, u_end;
  for (boost::tie(u_iter, u_end) = boost::vertices(graph);
      u_iter != u_end; ++u_iter) {
    om::common::SegID vertexSegmentID = segmentIDProperty[*u_iter];
    if (vertexSegmentID == segment->value()) {
      segmentIsFound = true;
      segmentVertex = *u_iter;
    } else if (parentSegment && vertexSegmentID == parentSegment->value()) {
      parentIsFound = true;
      parentIsFound = *u_iter;
    }
  }
  EXPECT_TRUE(segmentIsFound);
  if (parentSegment) {
    EXPECT_TRUE(parentIsFound);
    Edge edge;
    bool edgeIsFound;
    std::tie(edge, edgeIsFound) = boost::edge(segmentVertex, parentVertex, graph);
    EXPECT_TRUE(edgeIsFound);
    EXPECT_EQ(capacityProperty[edge], segment->getThreshold());
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

}

} //namespace boostgraph
} //namespace test

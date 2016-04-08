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

  SegmentProperty segmentProperty = boost::get(vertex_segment_t(), graph);
  Vertex rootSegmentVertex;
  boost::template graph_traits<Graph>::vertex_iterator u_iter, u_end;
  for (boost::tie(u_iter, u_end) = boost::vertices(graph); u_iter != u_end; ++u_iter) {
    const OmSegment* vertexSegment = segmentProperty[*u_iter];
    if (vertexSegment == segment1.get()) {
      rootSegmentVertex = *u_iter;
      break;
    }
  }

  ASSERT_TRUE(segmentProperty[rootSegmentVertex] == segment1.get());
  

}

} //namespace boostgraph
} //namespace test

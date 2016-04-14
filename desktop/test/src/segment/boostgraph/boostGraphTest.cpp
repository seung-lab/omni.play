#pragma once
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "segment/omSegment.h"
#include "segment/lowLevel/children.hpp"
#include "segment/boostgraph/types.hpp"
#include "segment/boostgraph/boostGraph.hpp"
#include "segment/boostgraph/validationUtils.hpp"
#include "segment/boostgraph/graphvizLabelWriter.hpp"
#include <boost/graph/graphviz.hpp>
#include "segment/testSetup.hpp"
#include <algorithm>

namespace test {
namespace boostgraph {

extern template void validateCutEdges<om::common::SegID, om::segment::UserEdge>(
    std::vector<std::tuple<om::common::SegID, om::common::SegID>>&,
    std::vector<om::segment::UserEdge>&, Graph&,
    std::function<om::common::SegID(om::segment::UserEdge)>,
    std::function<om::common::SegID(om::segment::UserEdge)>);
extern template void validateCutEdges<Vertex, Edge>(
  std::vector<std::tuple<Vertex, Vertex>>&, std::vector<Edge>&, Graph&,
  std::function<Vertex(Edge)>, std::function<Vertex(Edge)>); 

TEST(boostGraph, testBuildBasicGraph) {
  uint32_t numSegments = 100;
  BoostGraphTest boostGraphTest(numSegments);
  std::vector<std::unique_ptr<OmSegment>>& segments =
    boostGraphTest.generateBasicLine();

  BoostGraph& boostGraph = boostGraphTest.GetBoostGraph();
  Graph& graph = boostGraph.GetGraph();

  EXPECT_EQ(numSegments, boost::num_vertices(graph));
  for (auto& segment : segments) {
    validateSegmentBuilt(segment.get(), graph);
  }
}

TEST(boostGraph, testBuildComplexGraph) {
  uint32_t numSegments = 100;
  BoostGraphTest boostGraphTest(numSegments);
  std::vector<std::unique_ptr<OmSegment>>& segments =
    boostGraphTest.generateTrinaryTree(.5);

  BoostGraph& boostGraph = boostGraphTest.GetBoostGraph();
  Graph& graph = boostGraph.GetGraph();

  EXPECT_EQ(numSegments, boost::num_vertices(graph));
  for (auto& segment : segments) {
    validateSegmentBuilt(segment.get(), graph);
  }
}

TEST(boostGraph, testSingleSourceSinkSingle) {
  uint32_t numSegments = 100;
  BoostGraphTest boostGraphTest(numSegments);
  std::vector<std::unique_ptr<OmSegment>>& segments =
    boostGraphTest.generateBasicLine();

  BoostGraph& boostGraph = boostGraphTest.GetBoostGraph();
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

TEST(boostGraph, testMultiSourceSinkSingle) {
  uint32_t numSegments = 100;
  BoostGraphTest boostGraphTest(numSegments);
  std::vector<std::unique_ptr<OmSegment>>& segments =
    boostGraphTest.generateBasicLine();

  BoostGraph& boostGraph = boostGraphTest.GetBoostGraph();
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
  uint32_t numSegments = 4;
  BoostGraphTest boostGraphTest(numSegments);
  std::vector<std::unique_ptr<OmSegment>>& segments =
    boostGraphTest.generateBasicLine();

  BoostGraph& boostGraph = boostGraphTest.GetBoostGraph();
  Graph& graph = boostGraph.GetGraph();

  // set the edge between seg 2 and 3 to be the min cut edge
  ColorProperty colorProperty = boost::get(boost::vertex_color, graph);
  uint32_t pivot = 2;
  for (auto& segment : segments) {
    Vertex vertex = boostGraph.GetVertex(segment->value());
    colorProperty[vertex] = segment->value() <= pivot ? 
      BoostGraph::COLOR_SOURCE : BoostGraph::COLOR_SINK;
  }

  std::vector<Edge> actualEdges = boostGraph.GetMinCutEdges(
      boostGraph.GetVertex(segments[0]->value()));

  // check to make sure the cut edge is between seg 2 and 3
  std::vector<std::tuple<Vertex, Vertex>> expectedEdges;
  expectedEdges.emplace_back(boostGraph.GetVertex(segments[1]->value()),
      boostGraph.GetVertex(segments[2]->value()));

  std::function<Vertex(Edge)> sourceFunction, targetFunction;
  std::tie(sourceFunction, targetFunction) = getEdgeToVertexFunctions(graph);
  validateCutEdges(expectedEdges, actualEdges, graph, sourceFunction,
      targetFunction);
}

TEST(boostGraph, testFindMinCutEdgesNotSoBasic) {
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  uint32_t numSegments = 6;
  
  // prepare test data naming convention helps identify structure
  // i.e. parentID_childID_grandChildId_
  // segments with id 3 and 4 will have roots 1 and 2, respectively.
  std::tie(data, childrenList) = prepareSegmentData(numSegments);
  std::unique_ptr<OmSegment> segment1 =
    createSegment(1, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2 =
    createSegment(2, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2_3 =
    createSegment(3, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2_4 =
    createSegment(4, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2_4_5 =
    createSegment(5, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_6 =
    createSegment(6, data, mockChildren, childrenList);

  addToChildren(segment1.get(), segment1_2.get(), .5, mockChildren);
  addToChildren(segment1_2.get(), segment1_2_3.get(), .5, mockChildren);
  addToChildren(segment1_2.get(), segment1_2_4.get(), .5, mockChildren);
  addToChildren(segment1_2_4.get(), segment1_2_4_5.get(), .5, mockChildren);
  addToChildren(segment1.get(), segment1_6.get(), .5, mockChildren);

  boostGraph.BuildGraph(segment1.get());
  Graph& graph = boostGraph.GetGraph();

  // cut edges 2-4 and 1-6
  ColorProperty colorProperty = boost::get(boost::vertex_color, graph);
  SegmentIDProperty segmentIDProperty = boost::get(vertex_segmentID(), graph);
  colorProperty[boostGraph.GetVertex(segment1->value())] =
    BoostGraph::COLOR_SOURCE;
  colorProperty[boostGraph.GetVertex(segment1_2->value())] =
    BoostGraph::COLOR_SOURCE;
  colorProperty[boostGraph.GetVertex(segment1_2_3->value())] =
    BoostGraph::COLOR_SOURCE;
  colorProperty[boostGraph.GetVertex(segment1_2_4->value())] =
    BoostGraph::COLOR_SINK;
  colorProperty[boostGraph.GetVertex(segment1_2_4_5->value())] =
    BoostGraph::COLOR_SINK;
  colorProperty[boostGraph.GetVertex(segment1_6->value())] =
    BoostGraph::COLOR_SINK;

  std::vector<Edge> actualEdges = boostGraph.GetMinCutEdges(
      boostGraph.GetVertex(segment1->value()));

  // check to make sure the cut edge is between seg 2-4 and 1-6
  std::vector<std::tuple<Vertex, Vertex>> expectedEdges;
  expectedEdges.emplace_back(boostGraph.GetVertex(segment1_2->value()),
      boostGraph.GetVertex(segment1_2_4->value()));
  expectedEdges.emplace_back(boostGraph.GetVertex(segment1->value()),
      boostGraph.GetVertex(segment1_6->value()));

  std::function<Vertex(Edge)> sourceFunction, targetFunction;
  std::tie(sourceFunction, targetFunction) = getEdgeToVertexFunctions(graph);
  validateCutEdges(expectedEdges, actualEdges, graph, sourceFunction,
      targetFunction);
}

TEST(boostGraph, testMinCutBasic) {
  BoostGraphTest boostGraphTest;
  std::vector<std::unique_ptr<OmSegment>>& segments =
    boostGraphTest.generateBasicLine();

  BoostGraph& boostGraph = boostGraphTest.GetBoostGraph();
  Graph& graph = boostGraph.GetGraph();

  CapacityProperty capacityProperty = boost::get(boost::edge_capacity, graph);
  ASSERT_TRUE(setEdge(boostGraph.GetVertex(segments[1]->value()),
        boostGraph.GetVertex(segments[2]->value()),
        .00001, capacityProperty, graph));

  om::common::SegIDSet sources;
  sources.insert(segments[0]->value());
  om::common::SegIDSet sinks;
  sinks.insert(segments[segments.size() - 1]->value());

  std::vector<om::segment::UserEdge> cutUserEdges = boostGraph.MinCut(
      sources, sinks);

  // check to make sure the cut edge is between seg 2 and 3
  std::vector<std::tuple<om::common::SegID, om::common::SegID>> expectedEdges;
  expectedEdges.emplace_back(segments[1]->value(), segments[2]->value());

  std::function<om::common::SegID(om::segment::UserEdge)>
    sourceFunction, targetFunction;
  std::tie(sourceFunction, targetFunction) = getUserEdgeToSegIDFunctions();
  validateCutEdges(expectedEdges, cutUserEdges, graph, sourceFunction,
      targetFunction);
}

TEST(boostGraph, testMinCutTriangle) {
  BoostGraphTest boostGraphTest(12);
  std::vector<std::unique_ptr<OmSegment>>& segments =
    boostGraphTest.generateTrinaryTree(.5);

  BoostGraph& boostGraph = boostGraphTest.GetBoostGraph();
  Graph& graph = boostGraph.GetGraph();

  CapacityProperty capacityProperty = boost::get(boost::edge_capacity, graph);
  ASSERT_TRUE(setEdge(boostGraph.GetVertex(segments[0]->value()),
        boostGraph.GetVertex(segments[3]->value()),
        .0001, capacityProperty, graph));

  om::common::SegIDSet sources;
  sources.insert(segments[0]->value());
  om::common::SegIDSet sinks;
  sinks.insert(segments[segments.size() - 1]->value());

  std::vector<om::segment::UserEdge> cutUserEdges = boostGraph.MinCut(
      sources, sinks);

  // check to make sure the cut edge is between the vertices indexed at 0 and 3
  std::vector<std::tuple<om::common::SegID, om::common::SegID>> expectedEdges;
  expectedEdges.emplace_back(segments[0]->value(), segments[3]->value());

  std::function<om::common::SegID(om::segment::UserEdge)>
    sourceFunction, targetFunction;
  std::tie(sourceFunction, targetFunction) = getUserEdgeToSegIDFunctions();
  validateCutEdges(expectedEdges, cutUserEdges, graph, sourceFunction,
      targetFunction);
}

/*
 * Reference trinary tree diagram for the following tests
 * Labelled Using vertex INDEX NOT segID
 *                                         0
 *                                         ^
 *               /                         |                         \
 *              1                          2                          3
 *              ^                          ^                          ^
 *      /       |       \          /       |       \          /       |       \
 *     4        5        6        7        8        9        10      11       12
 *     ^        ^        ^        ^        ^        ^        ^        ^        ^
 *   / |  \   / |  \   / |  \   / |  \   / |  \   / |  \   / |  \   / |  \   / |  \
 *  13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39
 */

TEST(boostGraph, testMinCutMultiSourceSinkForceManyCuts) {
  BoostGraphTest boostGraphTest;
  std::vector<std::unique_ptr<OmSegment>>& segments =
    boostGraphTest.generateTrinaryTree(.5);

  BoostGraph& boostGraph = boostGraphTest.GetBoostGraph();
  Graph& graph = boostGraph.GetGraph();

  CapacityProperty capacityProperty = boost::get(boost::edge_capacity, graph);
  NameProperty nameProperty = boost::get(boost::vertex_name, graph);
  ColorProperty colorProperty = boost::get(boost::vertex_color, graph);

  // in this test case we force it to cut close to the source/sinks
  om::common::SegIDSet sources;
  sources.insert(segments[16]->value());
  sources.insert(segments[5]->value());
  sources.insert(segments[2]->value());
  om::common::SegIDSet sinks;
  sinks.insert(segments[17]->value());
  sinks.insert(segments[3]->value());
  sinks.insert(segments[11]->value());

  std::vector<om::segment::UserEdge> cutUserEdges = boostGraph.MinCut(
      sources, sinks);

  std::vector<std::tuple<om::common::SegID, om::common::SegID>> expectedEdges;
  expectedEdges.emplace_back(segments[0]->value(), segments[3]->value());
  expectedEdges.emplace_back(segments[5]->value(), segments[17]->value());

  std::function<om::common::SegID(om::segment::UserEdge)>
    sourceFunction, targetFunction;
  std::tie(sourceFunction, targetFunction) = getUserEdgeToSegIDFunctions();
  validateCutEdges(expectedEdges, cutUserEdges, graph, sourceFunction,
      targetFunction);

  //write_graphviz(std::cout, graph, make_label_writer_2(nameProperty, colorProperty),
      //make_label_writer(capacityProperty));
}

TEST(boostGraph, testMinCutMultiSourceSinkJump) {
  BoostGraphTest boostGraphTest;
  std::vector<std::unique_ptr<OmSegment>>& segments =
    boostGraphTest.generateTrinaryTree(.5);

  BoostGraph& boostGraph = boostGraphTest.GetBoostGraph();
  Graph& graph = boostGraph.GetGraph();

  CapacityProperty capacityProperty = boost::get(boost::edge_capacity, graph);
  NameProperty nameProperty = boost::get(boost::vertex_name, graph);
  ColorProperty colorProperty = boost::get(boost::vertex_color, graph);

  ASSERT_TRUE(setEdge(boostGraph.GetVertex(segments[1]->value()),
        boostGraph.GetVertex(segments[4]->value()),
        .0001, capacityProperty, graph));
  ASSERT_TRUE(setEdge(boostGraph.GetVertex(segments[3]->value()),
        boostGraph.GetVertex(segments[12]->value()),
        .0001, capacityProperty, graph));

  // in this test case we force it to cut close to the source/sinks
  om::common::SegIDSet sources;
  sources.insert(segments[13]->value());
  om::common::SegIDSet sinks;
  sinks.insert(segments[39]->value());

  std::vector<om::segment::UserEdge> cutUserEdges = boostGraph.MinCut(
      sources, sinks);

  std::vector<std::tuple<om::common::SegID, om::common::SegID>> expectedEdges;
  expectedEdges.emplace_back(segments[1]->value(), segments[4]->value());

  std::function<om::common::SegID(om::segment::UserEdge)>
    sourceFunction, targetFunction;
  std::tie(sourceFunction, targetFunction) = getUserEdgeToSegIDFunctions();
  validateCutEdges(expectedEdges, cutUserEdges, graph, sourceFunction,
      targetFunction);

  //write_graphviz(std::cout, graph, make_label_writer_2(nameProperty, colorProperty),
      //make_label_writer(capacityProperty));
}
} //namespace boostgraph
} //namespace test

#pragma once
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "segment/omSegment.h"
#include "segment/lowLevel/children.hpp"
#include "segment/boostgraph/types.hpp"
#include "segment/boostgraph/boostGraph.hpp"
#include "segment/boostgraph/validationUtils.hpp"
#include "segment/testSetup.hpp"
#include <algorithm>
#include <boost/graph/graphviz.hpp>

using namespace test::segment;
using namespace test::segment::boostgraph;

namespace test {
namespace boostGraph {

TEST(boostGraph, testBuildBasicGraph) {
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  uint32_t numSegments = 100;
  std::tie(data, childrenList) = prepareSegmentData(numSegments);

  std::vector<std::unique_ptr<OmSegment>> segments =
    getBasicLineGraph(numSegments, data, mockChildren, childrenList);

  boostGraph.BuildGraph(segments[0].get());
  Graph& graph = boostGraph.GetGraph();

  EXPECT_EQ(numSegments, boost::num_vertices(graph));
  for (auto& segment : segments) {
    validateSegmentBuilt(segment.get(), graph);
  }
}

TEST(boostGraph, testBuildComplexGraph) {
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  uint32_t numSegments = 100;
  std::tie(data, childrenList) = prepareSegmentData(numSegments);

  std::vector<std::unique_ptr<OmSegment>> segments =
    getTrinaryTreeGraph(numSegments, .23, data, mockChildren, childrenList);

  boostGraph.BuildGraph(segments[0].get());
  Graph& graph = boostGraph.GetGraph();

  EXPECT_EQ(numSegments, boost::num_vertices(graph));
  for (auto& segment : segments) {
    validateSegmentBuilt(segment.get(), graph);
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

TEST(boostGraph, testMultiSourceSinkSingle) {
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

  // set the edge between seg 2 and 3 is the weakest link
  CapacityProperty capacityProperty = boost::get(boost::edge_capacity, graph);
  Vertex segment2Vertex = boostGraph.GetVertex(segments[1]->value());
  Vertex segment3Vertex = boostGraph.GetVertex(segments[2]->value());
  Edge edge;
  bool edgeIsFound;
  std::tie(edge, edgeIsFound) =
    boost::edge(segment2Vertex, segment3Vertex, graph);
  ASSERT_TRUE(edgeIsFound);
  capacityProperty[edge] = .00001;

  om::common::SegIDSet sources;
  sources.insert(segments[0]->value());
  om::common::SegIDSet sinks;
  sinks.insert(segments[segments.size() - 1]->value());

  std::vector<om::segment::UserEdge> cutUserEdges = boostGraph.MinCut(
      sources, sinks);

  std::cout << cutUserEdges.size() << std::endl;
  for (auto edge : cutUserEdges) {
    std::cout << "I cut (" << edge.parentID << ", " << edge.childID << ")" << std::endl;
  }
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
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  uint32_t numSegments = 4;
  std::tie(data, childrenList) = prepareSegmentData(numSegments);

  std::vector<std::unique_ptr<OmSegment>> segments;
  for (int index = 0; index < numSegments; ++index) {
    // segment id = index + 1 since segment id 0 is not a valid id
    segments.push_back(createSegment(index + 1, data, mockChildren,
          childrenList));
  }
  // anything at index before numSegments/2 (index = 2, segmentId = 3) has
  // root of numSegments/2 
  int pivotIndex = numSegments/2; 

  // joining pivot as root to start
  for (int parentIndex = pivotIndex; parentIndex > 0; --parentIndex) {
    addToChildren(segments[parentIndex].get(),
        segments[parentIndex - 1].get(), segments[parentIndex]->value()/10.0,
        mockChildren);
  }
  // joining pivot as root to end
  for (int parentIndex = pivotIndex; parentIndex < numSegments - 1; ++parentIndex) {
    addToChildren(segments[parentIndex].get(),
        segments[parentIndex + 1].get(), segments[parentIndex]->value()/10.0,
        mockChildren);
  }

  boostGraph.BuildGraph(segments[pivotIndex].get());
  Graph& graph = boostGraph.GetGraph();

  // set the edge between seg 2 and 3 is the weakest link
  CapacityProperty capacityProperty = boost::get(boost::edge_capacity, graph);
  Vertex segment2Vertex = boostGraph.GetVertex(segments[1]->value());
  Vertex segment3Vertex = boostGraph.GetVertex(segments[2]->value());
  SegmentIDProperty segmentIDProperty = boost::get(vertex_segmentID(), graph);
  Edge edge;
  bool edgeIsFound;
  std::tie(edge, edgeIsFound) =
    boost::edge(segment2Vertex, segment3Vertex, graph);
  ASSERT_TRUE(edgeIsFound);
  capacityProperty[edge] = .00001;

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

template <class Name>
class label_writer {
 public:
  label_writer(Name name) : name_(name) {}
  template <class VertexOrEdge>
  void operator()(std::ostream& out, const VertexOrEdge& v) const {
    out << "[label=\"" << name_[v] << "\"]";
  }
private:
  Name name_;
};

template <class Prop1, class Prop2>
class label_writer_2 {
 public:
  label_writer_2(Prop1 prop1, Prop2 prop2) : prop1_(prop1), prop2_(prop2) {}
  template <class VertexOrEdge>
  void operator()(std::ostream& out, const VertexOrEdge& v) const {
    out << "[label=\"" << prop1_[v] << ", "  << prop2_[v] << "\"]";
  }
private:
  Prop1 prop1_;
  Prop2 prop2_;
};

template <class Prop1, class Prop2>
inline label_writer_2<Prop1, Prop2>
make_label_writer_2(Prop1 prop1, Prop2 prop2) {
  return label_writer_2<Prop1, Prop2>(prop1, prop2);
};

TEST(boostGraph, testMinCutMultiSourceSink) {
  testing::NiceMock<MockChildren> mockChildren;
  BoostGraph boostGraph(mockChildren);
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  uint32_t numSegments = 13;
  std::tie(data, childrenList) = prepareSegmentData(numSegments);

  std::vector<std::unique_ptr<OmSegment>> segments = getTrinaryTreeGraph(
      numSegments, .5, data,mockChildren,childrenList);

  //std::vector<std::unique_ptr<OmSegment>> segments;
  //for (int index = 0; index < numSegments; ++index) {
    //// segment id = index + 1 since segment id 0 is not a valid id
    //segments.push_back(createSegment(index + 1, data, mockChildren,
          //childrenList));
  //}
  // try to perform this cut (vertex index which is segmentid-1)
  //           5 (source)
  //         < 6 (source)
  //     4 < 7 (sink)
  // 3 <
  //     2 < 0 (sink)
  //         1 (sink)
  //
  //           6 (source)
  //         < 7 (source)
  //     5 < 8 (sink)
  // 4 <
  //     3 < 1 (sink)
  //         2 (sink)
  //
  //addToChildren(segments[3].get(), segments[4].get(), .8, mockChildren);
  //addToChildren(segments[4].get(), segments[5].get(), .8, mockChildren);
  //addToChildren(segments[4].get(), segments[6].get(), .8, mockChildren);
  //addToChildren(segments[4].get(), segments[7].get(), .8, mockChildren);
  //addToChildren(segments[3].get(), segments[2].get(), .8, mockChildren);
  //addToChildren(segments[2].get(), segments[0].get(), .8, mockChildren);
  //addToChildren(segments[2].get(), segments[1].get(), .8, mockChildren);

  boostGraph.BuildGraph(segments[0].get());
  Graph& graph = boostGraph.GetGraph();

  CapacityProperty capacityProperty = boost::get(boost::edge_capacity, graph);
  NameProperty nameProperty = boost::get(boost::vertex_name, graph);
  ColorProperty colorProperty = boost::get(boost::vertex_color, graph);
  //Vertex segment2Vertex = boostGraph.GetVertex(segments[3]->value());
  //Vertex segment3Vertex = boostGraph.GetVertex(segments[2]->value());
  //SegmentIDProperty segmentIDProperty = boost::get(vertex_segmentID(), graph);
  //Edge edge;
  //bool edgeIsFound;
  //std::tie(edge, edgeIsFound) =
    //boost::edge(segment2Vertex, segment3Vertex, graph);
  //ASSERT_TRUE(edgeIsFound);
  ////capacityProperty[edge] = .001;
  //std::tie(edge, edgeIsFound) =
    //boost::edge(segment3Vertex, segment2Vertex, graph);
  //ASSERT_TRUE(edgeIsFound);
  ////capacityProperty[edge] = .001;

  //om::common::SegIDSet sources;
  //sources.insert(segments[0]->value());
  //om::common::SegIDSet sinks;
  //sinks.insert(segments[3]->value());

  //std::vector<om::segment::UserEdge> cutUserEdges = boostGraph.MinCut(
      //sources, sinks);

  //std::cout << "trying to cut ";
  //for (auto edge : cutUserEdges) {
    //std::cout << "(" << edge.parentID << "," << edge.childID << ")";
  //}
  //std::cout << std::endl;

  write_graphviz(std::cout, graph, make_label_writer_2(nameProperty, colorProperty),
      make_label_writer(capacityProperty));
  // check to make sure the cut edge is between seg 2 and 3
  //std::vector<std::tuple<om::common::SegID, om::common::SegID>> expectedEdges;
  //expectedEdges.emplace_back(segments[1]->value(), segments[2]->value());

  //std::function<om::common::SegID(om::segment::UserEdge)>
    //sourceFunction, targetFunction;
  //std::tie(sourceFunction, targetFunction) = getUserEdgeToSegIDFunctions();
  //validateCutEdges(expectedEdges, cutUserEdges, graph, sourceFunction,
      //targetFunction);
}
} //namespace boostgraph
} //namespace test

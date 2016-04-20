#include "precomp.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "utility/segmentationDataWrapper.hpp"
#include "segment/minCut.hpp"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "segment/boostgraph/types.hpp"
#include "volume/omSegmentation.h"
#include "project/omProject.h"
#include "segment/boostgraph/types.hpp"
#include "segment/boostgraph/boostGraph.hpp"
#include "segment/testSetup.hpp"

const std::string fnp("../../test_data/build_project/meshTest.omni");

using namespace test::boostgraph;

namespace test {
namespace mincut {

TEST(minCut, testEmpty) {
  // necessary setup for OmSegments
  OmProject::New(QString::fromStdString(fnp));
  OmSegmentation* segmentation = &SegmentationDataWrapper().Create();

  OmSegments mockSegments(segmentation,
      std::unique_ptr<OmSegmentsImpl>(new MockSegmentsImpl(segmentation)));

  std::shared_ptr<BoostGraph> mockBoostGraph(
      new testing::NiceMock<MockBoostGraph>());
  MinCut minCut(mockSegments,
      std::make_shared<testing::NiceMock<MockBoostGraphFactory>>(
        mockBoostGraph));

  om::segment::UserEdge returnEdge = minCut.FindEdge(om::common::SegIDSet(),
      om::common::SegIDSet());
  EXPECT_FALSE(returnEdge.valid);
}

TEST(minCut, testIntersect) {
  // necessary setup for OmSegments
  OmProject::New(QString::fromStdString(fnp));
  OmSegmentation* segmentation = &SegmentationDataWrapper().Create();

  // set up mock children
  testing::NiceMock<MockChildren> mockChildren;

  // set up MockSegmentsImpl with ownership and retrieve it into a pointer
  std::unique_ptr<OmSegmentsImpl> 
    mockSegmentsPtr(new testing::NiceMock<MockSegmentsImpl>(segmentation));
  testing::NiceMock<MockSegmentsImpl>& mockSegmentsImpl 
    = static_cast<testing::NiceMock<MockSegmentsImpl>&>(*mockSegmentsPtr);

   //prepare test data naming convention helps identify structure
   //i.e. parentID_childID_grandChildId_
   //segments with id 3 and 4 will have roots 1 and 2, respectively.
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  std::tie(data, childrenList) = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(1, data);
  std::unique_ptr<OmSegment> segment2 = createSegment(2, data);
  std::unique_ptr<OmSegment> segment1_3 = createSegment(3, data);
  std::unique_ptr<OmSegment> segment2_4 = createSegment(4, data);

  // 2 root nodes
  connect(nullptr, segment1.get(), 0, mockSegmentsImpl);
  connect(nullptr, segment2.get(), 0, mockSegmentsImpl);
  connect(segment1.get(), segment1_3.get(), .5, mockSegmentsImpl);
  connect(segment2.get(), segment2_4.get(), .5, mockSegmentsImpl);

  // mockSegmentsImpl is no longer valid after move!
  OmSegments mockSegments(segmentation, std::move(mockSegmentsPtr));

  std::shared_ptr<BoostGraph> mockBoostGraph(new MockBoostGraph());
  MinCut minCut(mockSegments,
      std::make_shared<MockBoostGraphFactory>(mockBoostGraph));

  // prepare test inputs
  om::common::SegIDSet sources;
  om::common::SegIDSet sinks;
  sources.insert(segment1_3->value());
  sinks.insert(segment1_3->value());
  sinks.insert(segment2_4->value());

  // test
  om::segment::UserEdge returnEdge = minCut.FindEdge(sources, sinks);

  // verify
  EXPECT_FALSE(returnEdge.valid);
}

TEST(minCut, testNotSameRoot) {
  // necessary setup for OmSegments
  OmProject::New(QString::fromStdString(fnp));
  OmSegmentation* segmentation = &SegmentationDataWrapper().Create();

  // set up mock children
  testing::NiceMock<MockChildren> mockChildren;

  // set up MockSegmentsImpl with ownership and retrieve it into a pointer
  std::unique_ptr<OmSegmentsImpl> 
    mockSegmentsPtr(new testing::NiceMock<MockSegmentsImpl>(segmentation));
  testing::NiceMock<MockSegmentsImpl>& mockSegmentsImpl 
    = static_cast<testing::NiceMock<MockSegmentsImpl>&>(*mockSegmentsPtr);

   //prepare test data naming convention helps identify structure
   //i.e. parentID_childID_grandChildId_
   //segments with id 3 and 4 will have roots 1 and 2, respectively.
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  std::tie(data, childrenList) = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(1, data);
  std::unique_ptr<OmSegment> segment2 = createSegment(2, data);
  std::unique_ptr<OmSegment> segment1_3 = createSegment(3, data);
  std::unique_ptr<OmSegment> segment2_4 = createSegment(4, data);

  // 2 root nodes
  connect(nullptr, segment1.get(), 0, mockSegmentsImpl);
  connect(nullptr, segment2.get(), 0, mockSegmentsImpl);
  connect(segment1.get(), segment1_3.get(), .5, mockSegmentsImpl);
  connect(segment2.get(), segment2_4.get(), .5, mockSegmentsImpl);

  // mockSegmentsImpl is no longer valid after move!
  OmSegments mockSegments(segmentation, std::move(mockSegmentsPtr));

  std::shared_ptr<BoostGraph> mockBoostGraph(new MockBoostGraph());
  MinCut minCut(mockSegments,
      std::make_shared<MockBoostGraphFactory>(mockBoostGraph));

  // prepare test inputs
  om::common::SegIDSet sources;
  om::common::SegIDSet sinks;
  sources.insert(segment1_3->value());
  sinks.insert(segment2_4->value());

  // test
  om::segment::UserEdge returnEdge = minCut.FindEdge(sources, sinks);

  // verify
  EXPECT_FALSE(returnEdge.valid);
}

TEST(minCut, testNoEdgeFound) {
  // necessary setup for OmSegments
  OmProject::New(QString::fromStdString(fnp));
  OmSegmentation* segmentation = &SegmentationDataWrapper().Create();

  // set up mock children
  testing::NiceMock<MockChildren> mockChildren;

  // set up MockSegmentsImpl with ownership and retrieve it into a pointer
  std::unique_ptr<OmSegmentsImpl> 
    mockSegmentsPtr(new testing::NiceMock<MockSegmentsImpl>(segmentation));
  testing::NiceMock<MockSegmentsImpl>& mockSegmentsImpl 
    = static_cast<testing::NiceMock<MockSegmentsImpl>&>(*mockSegmentsPtr);

  //prepare test data naming convention helps identify structure
  //i.e. parentID_childID_grandChildId_
  //segments with id 3 and 4 will have roots 1 and 2, respectively.
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  std::tie(data, childrenList) = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(1, data);
  std::unique_ptr<OmSegment> segment1_2 = createSegment(2, data);
  std::unique_ptr<OmSegment> segment1_3 = createSegment(3, data);

  // 1 root nodes
  connect(nullptr, segment1.get(), 0, mockSegmentsImpl);
  connect(segment1.get(), segment1_2.get(), .5, mockSegmentsImpl);
  connect(segment1.get(), segment1_3.get(), .5, mockSegmentsImpl);

  // mockSegmentsImpl is no longer valid after move!
  OmSegments mockSegments(segmentation, std::move(mockSegmentsPtr));

  std::shared_ptr<BoostGraph> mockBoostGraph(
      new testing::NiceMock<MockBoostGraph>());
  MinCut minCut(mockSegments,
      std::make_shared<testing::NiceMock<MockBoostGraphFactory>>(
        mockBoostGraph));

  // prepare test inputs
  om::common::SegIDSet sources;
  om::common::SegIDSet sinks;
  sources.insert(segment1_2->value());
  sinks.insert(segment1_3->value());

  // test
  om::segment::UserEdge returnEdge = minCut.FindEdge(sources, sinks);

  // verify
  EXPECT_FALSE(returnEdge.valid);
}

/*
 * Tests BOTH findEdge and findEdges to make sure the value is correct!
 */
TEST(minCut, testEdgesReturn) {
  // necessary setup for OmSegments
  OmProject::New(QString::fromStdString(fnp));
  OmSegmentation* segmentation = &SegmentationDataWrapper().Create();

  // set up mock children
  testing::NiceMock<MockChildren> mockChildren;

  // set up MockSegmentsImpl with ownership and retrieve it into a pointer
  std::unique_ptr<OmSegmentsImpl> 
    mockSegmentsPtr(new testing::NiceMock<MockSegmentsImpl>(segmentation));
  testing::NiceMock<MockSegmentsImpl>& mockSegmentsImpl 
    = static_cast<testing::NiceMock<MockSegmentsImpl>&>(*mockSegmentsPtr);

   //prepare test data naming convention helps identify structure
   //i.e. parentID_childID_grandChildId_
   //segments with id 3 and 4 will have roots 1 and 2, respectively.
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  std::tie(data, childrenList) = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(1, data);
  std::unique_ptr<OmSegment> segment1_2 = createSegment(2, data);
  std::unique_ptr<OmSegment> segment1_3 = createSegment(3, data);

  // 1 root nodes
  connect(nullptr, segment1.get(), .0, mockSegmentsImpl);
  connect(segment1.get(), segment1_2.get(), .5, mockSegmentsImpl);
  connect(segment1.get(), segment1_3.get(), .5, mockSegmentsImpl);

  // mockSegmentsImpl is no longer valid after move!
  OmSegments mockSegments(segmentation, std::move(mockSegmentsPtr));

  std::vector<om::segment::Edge> minCutEdges;
  std::shared_ptr<MockBoostGraph> mockBoostGraph(
      new testing::NiceMock<MockBoostGraph>());

  MinCut minCut(mockSegments,
      std::make_shared<testing::NiceMock<MockBoostGraphFactory>>(
        mockBoostGraph));

  // prepare test inputs
  om::common::SegIDSet sources;
  om::common::SegIDSet sinks;
  sources.insert(segment1_2->value());
  sinks.insert(segment1_3->value());

  om::segment::UserEdge userEdge;
  std::vector<om::segment::UserEdge> userEdges;

  // Valid and parent child
  minCutEdges.push_back({.number = 0, .node1ID = segment1->value(),
      .node2ID = segment1_2->value()});
  EXPECT_CALL(*mockBoostGraph,
      MinCut(testing::_, testing::_))
    .WillOnce(testing::Return(minCutEdges))
    .WillOnce(testing::Return(minCutEdges));
  userEdge = minCut.FindEdge(sources, sinks);
  EXPECT_EQ(segment1->value(), userEdge.parentID);
  EXPECT_EQ(segment1_2->value(), userEdge.childID);
  EXPECT_TRUE(userEdge.valid);

  userEdges = minCut.FindEdges(sources, sinks);
  EXPECT_EQ(1, userEdges.size());
  if (userEdges.size() == 1) {
    userEdge = userEdges[0];
    EXPECT_EQ(segment1->value(), userEdge.parentID);
    EXPECT_EQ(segment1_2->value(), userEdge.childID);
    EXPECT_TRUE(userEdge.valid);
  }

  // valid but child and parent are reversed
  minCutEdges.clear();
  minCutEdges.push_back({.number = 0, .node1ID = segment1_2->value(),
      .node2ID = segment1->value()});
  EXPECT_CALL(*mockBoostGraph,
      MinCut(testing::_, testing::_))
    .WillOnce(testing::Return(minCutEdges))
    .WillOnce(testing::Return(minCutEdges));
  userEdge = minCut.FindEdge(sources, sinks);
  EXPECT_EQ(segment1->value(), userEdge.parentID);
  EXPECT_EQ(segment1_2->value(), userEdge.childID);
  EXPECT_TRUE(userEdge.valid);

  userEdges = minCut.FindEdges(sources, sinks);
  EXPECT_EQ(1, userEdges.size());
  if (userEdges.size() == 1) {
    userEdge = userEdges[0];
    EXPECT_EQ(segment1->value(), userEdge.parentID);
    EXPECT_EQ(segment1_2->value(), userEdge.childID);
    EXPECT_TRUE(userEdge.valid);
  }

  // Multiple userEdges
  minCutEdges.clear();
  minCutEdges.push_back({.number = 0, .node1ID = segment1->value(),
      .node2ID = segment1_2->value()});
  minCutEdges.push_back({.number = 0, .node1ID = segment1->value(),
      .node2ID = segment1_3->value()});
  EXPECT_CALL(*mockBoostGraph,
      MinCut(testing::_, testing::_))
    .WillOnce(testing::Return(minCutEdges))
    .WillOnce(testing::Return(minCutEdges));
  userEdge = minCut.FindEdge(sources, sinks);
  EXPECT_EQ(segment1->value(), userEdge.parentID);
  EXPECT_EQ(segment1_2->value(), userEdge.childID);
  EXPECT_TRUE(userEdge.valid);

  userEdges = minCut.FindEdges(sources, sinks);
  EXPECT_EQ(2, userEdges.size());
  if (userEdges.size() == 2) {
    userEdge = userEdges[0];
    EXPECT_EQ(segment1->value(), userEdge.parentID);
    EXPECT_EQ(segment1_2->value(), userEdge.childID);
    EXPECT_TRUE(userEdge.valid);

    userEdge = userEdges[1];
    EXPECT_EQ(segment1->value(), userEdge.parentID);
    EXPECT_EQ(segment1_3->value(), userEdge.childID);
    EXPECT_TRUE(userEdge.valid);
  }

  // Multiple userEdges but only the second is valid
  minCutEdges.clear();
  minCutEdges.push_back({.number = 0, .node1ID = 0,
      .node2ID = 0});
  minCutEdges.push_back({.number = 0, .node1ID = segment1->value(),
      .node2ID = segment1_3->value()});
  EXPECT_CALL(*mockBoostGraph,
      MinCut(testing::_, testing::_))
    .WillOnce(testing::Return(minCutEdges))
    .WillOnce(testing::Return(minCutEdges));
  userEdge = minCut.FindEdge(sources, sinks);
  EXPECT_EQ(segment1->value(), userEdge.parentID);
  EXPECT_EQ(segment1_3->value(), userEdge.childID);
  EXPECT_TRUE(userEdge.valid);

  userEdges = minCut.FindEdges(sources, sinks);
  EXPECT_EQ(1, userEdges.size());
  if (userEdges.size() == 1) {
    userEdge = userEdges[0];
    EXPECT_EQ(segment1->value(), userEdge.parentID);
    EXPECT_EQ(segment1_3->value(), userEdge.childID);
    EXPECT_TRUE(userEdge.valid);
  }

  // Test invalid node 1
  minCutEdges.clear();
  minCutEdges.push_back({.number = 0, .node1ID = 0, .node2ID = segment1_2->value()});
  EXPECT_CALL(*mockBoostGraph,
      MinCut(testing::_, testing::_))
    .WillOnce(testing::Return(minCutEdges))
    .WillOnce(testing::Return(minCutEdges));
  EXPECT_FALSE(minCut.FindEdge(sources, sinks).valid);

  userEdges = minCut.FindEdges(sources, sinks);
  EXPECT_EQ(0, userEdges.size());

  // Test invalid node 2
  minCutEdges.clear();
  minCutEdges.push_back({.number = 0, .node1ID = segment1->value(), .node2ID = 0});
  EXPECT_CALL(*mockBoostGraph,
      MinCut(testing::_, testing::_))
    .WillOnce(testing::Return(minCutEdges))
    .WillOnce(testing::Return(minCutEdges));
  EXPECT_FALSE(minCut.FindEdge(sources, sinks).valid);

  userEdges = minCut.FindEdges(sources, sinks);
  EXPECT_EQ(0, userEdges.size());

  // invalid node 1 and 2
  minCutEdges.clear();
  minCutEdges.push_back({.number = 0, .node1ID = 0, .node2ID = 0});
  EXPECT_CALL(*mockBoostGraph,
      MinCut(testing::_, testing::_))
    .WillOnce(testing::Return(minCutEdges))
    .WillOnce(testing::Return(minCutEdges));
  EXPECT_FALSE(minCut.FindEdge(sources, sinks).valid);

  userEdges = minCut.FindEdges(sources, sinks);
  EXPECT_EQ(0, userEdges.size());
}

} //namespace mincut
} //namespace test

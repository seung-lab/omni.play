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

/*
 * Tests minCut class.
 * WARNING: please topologically add nodes using helper functions because
 * because omSegmentsImpl and Children ONLY mock the return values.
 * i.e. add roots first because FindRoot will only update based on previous root
 */
const std::string fnp("../../test_data/build_project/meshTest.omni");

using namespace test::segment;

namespace test {
namespace minCut {

TEST(minCut, testEmpty) {
  // necessary setup for OmSegments
  OmProject::New(QString::fromStdString(fnp));
  OmSegmentation* segmentation = &SegmentationDataWrapper().Create();

  std::cout << "Create mock segments"<< std::endl;
  OmSegments mockSegments(segmentation,
      std::unique_ptr<OmSegmentsImpl>(new MockSegmentsImpl(segmentation)));

  std::cout << "Create mincut"<< std::endl;
  MinCut minCut(mockSegments, MockBoostGraphFactory());
  std::cout << "Create perform mincut"<< std::endl;
  om::segment::UserEdge returnEdge = minCut.FindEdge(om::common::SegIDSet(),
      om::common::SegIDSet());
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

  /*
   * prepare test data naming convention helps identify structure
   * i.e. parentID_childID_grandChildId_
   * segments with id 3 and 4 will have roots 1 and 2, respectively.
   */
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  std::tie(data, childrenList) = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(1, data);
  std::unique_ptr<OmSegment> segment2 = createSegment(2, data);
  std::unique_ptr<OmSegment> segment1_3 = createSegment(3, data);
  std::unique_ptr<OmSegment> segment2_4 = createSegment(4, data);

  // 2 root nodes
  establishRoot(nullptr, segment1.get(), mockSegmentsImpl);
  establishRoot(nullptr, segment2.get(), mockSegmentsImpl);
  establishRoot(segment1.get(), segment1_3.get(), mockSegmentsImpl);
  establishRoot(segment2.get(), segment2_4.get(), mockSegmentsImpl);

  // mockSegmentsImpl is no longer valid after move!
  OmSegments mockSegments(segmentation, std::move(mockSegmentsPtr));

  // prepare test inputs
  om::common::SegIDSet sources;
  om::common::SegIDSet sinks;
  sources.insert(segment1_3->value());
  sinks.insert(segment2_4->value());
  MinCut minCut(mockSegments, MockBoostGraphFactory());

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

  /*
   * prepare test data naming convention helps identify structure
   * i.e. parentID_childID_grandChildId_
   * segments with id 3 and 4 will have roots 1 and 2, respectively.
   */
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  std::tie(data, childrenList) = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(1, data);
  std::unique_ptr<OmSegment> segment1_2 = createSegment(2, data);
  std::unique_ptr<OmSegment> segment1_3 = createSegment(3, data);

  // 1 root nodes
  establishRoot(nullptr, segment1.get(), mockSegmentsImpl);
  establishRoot(segment1.get(), segment1_2.get(), mockSegmentsImpl);
  establishRoot(segment1.get(), segment1_3.get(), mockSegmentsImpl);

  // mockSegmentsImpl is no longer valid after move!
  OmSegments mockSegments(segmentation, std::move(mockSegmentsPtr));

  // prepare test inputs
  om::common::SegIDSet sources;
  om::common::SegIDSet sinks;
  sources.insert(segment1_2->value());
  sinks.insert(segment1_3->value());
  MinCut minCut(mockSegments, MockBoostGraphFactory());

  // test
  om::segment::UserEdge returnEdge = minCut.FindEdge(sources, sinks);

  // verify
  EXPECT_FALSE(returnEdge.valid);
}

TEST(minCut, testEdgeFoundSuccess) {
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

  /*
   * prepare test data naming convention helps identify structure
   * i.e. parentID_childID_grandChildId_
   * segments with id 3 and 4 will have roots 1 and 2, respectively.
   */
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  std::tie(data, childrenList) = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(1, data);
  std::unique_ptr<OmSegment> segment1_2 = createSegment(2, data);
  std::unique_ptr<OmSegment> segment1_3 = createSegment(3, data);

  // 1 root nodes
  establishRoot(nullptr, segment1.get(), mockSegmentsImpl);
  establishRoot(segment1.get(), segment1_2.get(), mockSegmentsImpl);
  establishRoot(segment1.get(), segment1_3.get(), mockSegmentsImpl);

  // mockSegmentsImpl is no longer valid after move!
  OmSegments mockSegments(segmentation, std::move(mockSegmentsPtr));

  om::segment::UserEdge userEdge;
  userEdge.valid = true;
  std::vector<om::segment::UserEdge> validEdges;
  validEdges.push_back(userEdge);
  std::shared_ptr<MockBoostGraph> mockBoostGraph
    = std::make_shared<MockBoostGraph>();
  EXPECT_CALL(*mockBoostGraph, MinCut(testing::_, testing::_))
      .WillRepeatedly(testing::Return(validEdges));

  // prepare test inputs
  om::common::SegIDSet sources;
  om::common::SegIDSet sinks;
  sources.insert(segment1_2->value());
  sinks.insert(segment1_3->value());
  MinCut minCut(mockSegments, MockBoostGraphFactory(mockBoostGraph));

  // test
  om::segment::UserEdge returnEdge = minCut.FindEdge(sources, sinks);

  // verify
  EXPECT_TRUE(returnEdge.valid);
}

} //namespace mincut
} //namespace test

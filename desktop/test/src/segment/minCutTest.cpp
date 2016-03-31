#include "precomp.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "segment/minCut.hpp"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "segment/lowLevel/children.hpp"
#include "volume/omSegmentation.h"
#include "project/omProject.h"
#include "utility/boostSegmentGraph.hpp"

const std::string fnp("../../test_data/build_project/meshTest.omni");

namespace test {
namespace minCut {

om::coords::VolumeSystem VOLUME_SYSTEM;
om::common::SegListType SEG_LIST_TYPE = om::common::SegListType::WORKING;

class MockSegmentsImpl : public OmSegmentsImpl {
 public:
  MockSegmentsImpl(OmSegmentation* segmentation)
    : OmSegmentsImpl(segmentation) {}

  MOCK_METHOD1(FindRoot, OmSegment*(const om::common::SegID));
  MOCK_METHOD1(FindRoot, OmSegment*(OmSegment*));
  MOCK_METHOD1(FindRootID, om::common::SegID(const om::common::SegID));
  MOCK_METHOD1(FindRootID, om::common::SegID(OmSegment*));
};

class MockChildrenImpl : public om::segment::Children {
 public:
  MockChildrenImpl() : Children(0) {}

  MOCK_METHOD1(GetChildren, const std::set<OmSegment*>(const om::common::SegID));
  MOCK_METHOD1(GetChildren, const std::set<OmSegment*>(const OmSegment*));
};

std::vector<om::segment::Data> prepareSegmentData(int numSegments) {
  std::vector<om::segment::Data> data;
  for (om::common::SegID i = 1; i < numSegments; i++) {
    data.push_back({.value = i});
  }
  return data;
}

std::unique_ptr<OmSegment> createSegment(std::vector<om::segment::Data> data,
    om::common::SegID segID) {
  if (segID > 0 && segID < data.size()) {
    return std::make_unique<OmSegment>(
        data[segID], SEG_LIST_TYPE, VOLUME_SYSTEM);
  } else {
    return std::unique_ptr<OmSegment>();
  }
}

/*
 * Establish the root in findRoot* functions for the given child.
 * Do this by trying to first getting the parent's root.
 * If the parent doesn't have a root (i.e. no parent), then set the root to itself
 */
void establishRoot(OmSegment* parent, OmSegment* child,
    MockSegmentsImpl& mockSegments) {
  OmSegment* rootSegment = mockSegments.FindRoot(parent);

  if (!rootSegment) {
    rootSegment = child;
  }

  EXPECT_CALL(mockSegments, FindRoot(child->value()))
    .WillRepeatedly(testing::Return(rootSegment));
  EXPECT_CALL(mockSegments, FindRoot(child->value()))
    .WillRepeatedly(testing::Return(rootSegment));
  EXPECT_CALL(mockSegments, FindRootID(child))
    .WillRepeatedly(testing::Return(rootSegment->value()));
  EXPECT_CALL(mockSegments, FindRootID(child))
    .WillRepeatedly(testing::Return(rootSegment->value()));
}

/*
 * Make sure that the children list of the parent now includes this child.
 */
void establishChildren(OmSegment* parent, OmSegment* child,
    MockChildrenImpl& mockChildren) {
  // set parent here TODO
  std::set<OmSegment*> children = mockChildren.GetChildren(parent);
  if (children.empty()) {
    EXPECT_CALL(mockChildren, GetChildren(parent))
      .WillRepeatedly(testing::Return(children));
    EXPECT_CALL(mockChildren, GetChildren(parent->value()))
      .WillRepeatedly(testing::Return(children));
  }
}

void connectSegment(OmSegment* parent, OmSegment* child, double threshold,
    MockSegmentsImpl& mockSegments, MockChildrenImpl& mockChildren) {
  establishRoot(parent, child, mockSegments);
  establishChildren(parent, child, mockChildren);
}

TEST(minCut, testEmpty) {
  // necessary setup for OmSegments
  OmProject::New(QString::fromStdString(fnp));
  OmSegmentation* segmentation = &SegmentationDataWrapper().Create();

  OmSegments mockSegments(segmentation,
      std::unique_ptr<OmSegmentsImpl>(new MockSegmentsImpl(segmentation)));

  MinCut minCut(mockSegments);
  om::segment::UserEdge returnEdge = minCut.findEdge(om::common::SegIDSet(),
      om::common::SegIDSet());
  EXPECT_FALSE(returnEdge.valid);
}

TEST(minCut, testNotSameRoot) {
  // necessary setup for OmSegments
  OmProject::New(QString::fromStdString(fnp));
  OmSegmentation* segmentation = &SegmentationDataWrapper().Create();

  // set up MockSegmentsImpl with ownership and retrieve underlying mockSegmentsImpl
  std::unique_ptr<OmSegmentsImpl> 
    mockSegmentsPtr(new MockSegmentsImpl(segmentation));
  MockSegmentsImpl* mockSegments = static_cast<MockSegmentsImpl*>(mockSegmentsPtr.get());

  // prepare test data
  // segmentds with id 3 and 4 will have roots 1 and 2, respectively.
  std::vector<om::segment::Data> data = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(data, 1);
  std::unique_ptr<OmSegment> segment2 = createSegment(data, 2);
  std::unique_ptr<OmSegment> segment1_1 = createSegment(data, 3);
  std::unique_ptr<OmSegment> segment2_1 = createSegment(data, 4);

  connectSegment(*segment1, *segment1_1);
  connectSegment(*segment1, *segment1_2);

  OmSegments mockSegments(segmentation, std::move(mockSegmentsImpl));
  // mockSegmentsImpl is no longer valid after move!

  // prepare test inputs
  om::common::SegIDSet sources;
  om::common::SegIDSet sinks;
  sources.insert(3);
  sinks.insert(4);
  MinCut minCut(mockSegments);

  // test
  om::segment::UserEdge returnEdge = minCut.findEdge(sources, sinks);

  // verify
  EXPECT_FALSE(returnEdge.valid);
}

TEST(minCut, findMinCutBoostGraph) {
}

} //namespace mincut
} //namespace test

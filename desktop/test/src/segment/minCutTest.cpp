#include "precomp.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "segment/minCut.hpp"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "segment/lowLevel/children.hpp"
#include "volume/omSegmentation.h"
#include "project/omProject.h"
#include "utility/boost/segmentGraph/types.hpp"

/*
 * Tests minCut class.
 * WARNING: please topologically add nodes using helper functions because
 * because omSegmentsImpl and Children ONLY mock the return values.
 * i.e. add roots first because FindRoot will only update based on previous root
 */
const std::string fnp("../../test_data/build_project/meshTest.omni");

namespace test {
namespace minCut {

om::coords::VolumeSystem VOLUME_SYSTEM;
om::common::SegListType SEG_LIST_TYPE = om::common::SegListType::WORKING;

typedef std::vector<std::set<OmSegment*>> ChildrenList;

class MockSegmentsImpl : public OmSegmentsImpl {
 public:
  MockSegmentsImpl(OmSegmentation* segmentation)
    : OmSegmentsImpl(segmentation) {
    }

  MOCK_METHOD1(FindRoot, OmSegment*(const om::common::SegID));
  MOCK_METHOD1(FindRoot, OmSegment*(OmSegment*));
  MOCK_METHOD1(FindRootID, om::common::SegID(const om::common::SegID));
  MOCK_METHOD1(FindRootID, om::common::SegID(OmSegment*));
};

class MockChildren : public om::segment::Children {
 public:
  MockChildren() : Children(0) {
      ON_CALL(*this, GetChildren(testing::An<om::common::SegID>()))
        .WillByDefault(testing::ReturnRef(EMPTY_CHILDREN));
      ON_CALL(*this, GetChildren(testing::A<const OmSegment*>()))
        .WillByDefault(testing::ReturnRef(EMPTY_CHILDREN));
  }

  MOCK_CONST_METHOD1(GetChildren,
      const std::set<OmSegment*>&(const om::common::SegID));
  MOCK_CONST_METHOD1(GetChildren,
      const std::set<OmSegment*>&(const OmSegment*));

 private:
  std::set<OmSegment*> EMPTY_CHILDREN;
};

/*
 * create segment data and it's children set
 */
std::tuple<std::vector<om::segment::Data>, ChildrenList>
    prepareSegmentData(int numSegments) {
  std::vector<om::segment::Data> data;
  ChildrenList children;
  for (om::common::SegID i = 0; i < numSegments; i++) {
    data.push_back({.value = i + 1});
    children.emplace_back();
  }
  return std::make_tuple(data,children);
}

/*
 * This void assertion function is only here because google test ONLY supports 
 * asserts within void functions!
 */
void checkIndex(std::vector<om::segment::Data>& data, om::common::SegID segID) {
  ASSERT_TRUE((segID > 0 && segID < data.size() + 1))
    << " Created out of bounds segment " <<
    " Data is of size " << data.size() << " but requested " << segID;
}

/*
 * Create a segment and set children list in the mockchildren
 */
std::unique_ptr<OmSegment> createSegment(om::common::SegID segID, 
    std::vector<om::segment::Data>& data,
    MockChildren& mockChildren,
    ChildrenList& childrenList) {
  checkIndex(data, segID);
  // offset data by -1 because in this unit test we choose not to express segment 0
  std::unique_ptr<OmSegment> ptr(
      new OmSegment(data[segID - 1], SEG_LIST_TYPE, VOLUME_SYSTEM));

  // set up the mockchildren call to reference the correct data
  ON_CALL(mockChildren, GetChildren(
        testing::TypedEq<om::common::SegID>(ptr->value())))
    .WillByDefault(testing::ReturnRef(childrenList[segID - 1]));
  ON_CALL(mockChildren, GetChildren(
        testing::TypedEq<const OmSegment*>(ptr.get())))
    .WillByDefault(testing::ReturnRef(childrenList[segID - 1]));
  return ptr;
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

  ON_CALL(mockSegments, FindRoot(
        testing::TypedEq<om::common::SegID>(child->value())))
    .WillByDefault(testing::Return(rootSegment));
  ON_CALL(mockSegments, FindRoot(testing::TypedEq<OmSegment*>(child)))
    .WillByDefault(testing::Return(rootSegment));
  ON_CALL(mockSegments, FindRootID(
        testing::TypedEq<om::common::SegID>(child->value())))
    .WillByDefault(testing::Return(rootSegment->value()));
  ON_CALL(mockSegments, FindRootID(testing::TypedEq<OmSegment*>(child)))
    .WillByDefault(testing::Return(rootSegment->value()));
}

/*
 * Make sure that the children list of the parent now includes this child.
 */
void addToChildren(OmSegment* parent, OmSegment* child, double threshold,
    MockChildren& mockChildren) {
  if (!parent) {
    return;
  }
  const std::set<OmSegment*>& children = mockChildren.GetChildren(parent);
  const_cast<std::set<OmSegment*>&>(children).insert(child);
}

void connectSegment(OmSegment* parent, OmSegment* child, double threshold, 
    MockSegmentsImpl& mockSegments, MockChildren& mockChildren) {
  establishRoot(parent, child, mockSegments);
  addToChildren(parent, child, threshold, mockChildren);
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
  std::unique_ptr<OmSegment> segment1 =
    createSegment(1, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment2 =
    createSegment(2, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_3 =
    createSegment(3, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment2_4 =
    createSegment(4, data, mockChildren, childrenList);

  // 2 root nodes
  connectSegment(nullptr, segment1.get(), 0, mockSegmentsImpl, mockChildren);
  connectSegment(nullptr, segment2.get(), 0, mockSegmentsImpl, mockChildren);
  connectSegment(segment1.get(), segment1_3.get(), .5, mockSegmentsImpl, mockChildren);
  connectSegment(segment2.get(), segment2_4.get(), .5, mockSegmentsImpl, mockChildren);

  // mockSegmentsImpl is no longer valid after move!
  OmSegments mockSegments(segmentation, std::move(mockSegmentsPtr));

  // prepare test inputs
  om::common::SegIDSet sources;
  om::common::SegIDSet sinks;
  sources.insert(segment1_3->value());
  sinks.insert(segment2_4->value());
  MinCut minCut(mockSegments);

  // test
  om::segment::UserEdge returnEdge = minCut.findEdge(sources, sinks);

  // verify
  EXPECT_FALSE(returnEdge.valid);
}

TEST(minCut, testConvertToGraph) {
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
   */
  std::vector<om::segment::Data> data;
  std::vector<std::set<OmSegment*>> childrenList;
  std::tie(data, childrenList) = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 =
    createSegment(1, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2 =
    createSegment(2, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2_3 =
    createSegment(3, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_2_4 =
    createSegment(4, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_5 =
    createSegment(5, data, mockChildren, childrenList);
  std::unique_ptr<OmSegment> segment1_5_6 =
    createSegment(6, data, mockChildren, childrenList);

  // thresholds weights are 1/10 of the childID
  connectSegment(nullptr, segment1.get(), 0, mockSegmentsImpl, mockChildren);
  connectSegment(segment1.get(), segment1_2.get(), segment1_2->value()/10.0,
      mockSegmentsImpl, mockChildren);
  connectSegment(segment1_2.get(), segment1_2_3.get(), segment1_2_3->value()/10.0,
      mockSegmentsImpl, mockChildren);
  connectSegment(segment1_2.get(), segment1_2_4.get(), segment1_2_4->value()/10.0,
      mockSegmentsImpl, mockChildren);
  connectSegment(segment1_5.get(), segment1_5.get(), segment1_5->value()/10.0,
      mockSegmentsImpl, mockChildren);

  // mockSegmentsImpl is no longer valid after move!
  OmSegments mockSegments(segmentation, std::move(mockSegmentsPtr));

  // prepare test inputs
  MinCut minCut(mockSegments);

  // test
  Graph g = minCut.createGraph(segment1.get());

  // verify
  EXPECT_FALSE(returnEdge.valid);
}

} //namespace mincut
} //namespace test

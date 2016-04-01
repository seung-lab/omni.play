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
        .WillByDefault(testing::Return(std::set<OmSegment*>()));
      ON_CALL(*this, GetChildren(testing::A<const OmSegment*>()))
        .WillByDefault(testing::Return(std::set<OmSegment*>()));
  }

  MOCK_METHOD1(GetChildren, const std::set<OmSegment*>(const om::common::SegID));
  MOCK_METHOD1(GetChildren, const std::set<OmSegment*>(const OmSegment*));
};

/*
 * Create test segment data but skip id 0
 */
std::vector<om::segment::Data> prepareSegmentData(int numSegments) {
  std::vector<om::segment::Data> data;
  for (om::common::SegID i = 0; i < numSegments; i++) {
    data.push_back({.value = i + 1});
  }
  return data;
}

/*
 * This void assertion function is only here because google test supports asserts
 * within void functions!
 */
void checkIndex(std::vector<om::segment::Data>& data, om::common::SegID segID) {
  ASSERT_TRUE((segID > 0 && segID < data.size() + 1))
    << " Created out of bounds segment " <<
    " Data is of size " << data.size() << " but requested " << segID;
}
std::unique_ptr<OmSegment> createSegment(std::vector<om::segment::Data>& data,
    om::common::SegID segID) {
  checkIndex(data, segID);
  // offset data by -1 because in this unit test we choose not to express segment 0
  std::unique_ptr<OmSegment> ptr(new OmSegment(data[segID - 1], SEG_LIST_TYPE, VOLUME_SYSTEM));
  std::cout << "creating segment " << segID << " " << ptr->value() << std::endl;
  return ptr;
      
}

/*
 * Establish the root in findRoot* functions for the given child.
 * Do this by trying to first getting the parent's root.
 * If the parent doesn't have a root (i.e. no parent), then set the root to itself
 */
void establishRoot(OmSegment* parent, OmSegment* child,
    MockSegmentsImpl& mockSegments) {
  std::cout << "trying to find root" << std::endl;
  OmSegment* rootSegment = mockSegments.FindRoot(parent);

  std::cout << "finished finding root" << std::endl;
  if (!rootSegment) {
    std::cout << "no root found " << std::endl;
    rootSegment = child;
  }

  std::cout << "Establishing root child :" << rootSegment << " " << rootSegment->value() << std::endl;
  ON_CALL(mockSegments, FindRoot(testing::TypedEq<om::common::SegID>(child->value())))
    .WillByDefault(testing::Return(rootSegment));
  std::cout << "Establishing root finsihed findroot by id" << std::endl;
  ON_CALL(mockSegments, FindRoot(testing::TypedEq<OmSegment*>(child)))
    .WillByDefault(testing::Return(rootSegment));
  std::cout << "Establishing root finsihed findroot by pointer" << std::endl;
  ON_CALL(mockSegments, FindRootID(testing::TypedEq<om::common::SegID>(child->value())))
    .WillByDefault(testing::Return(rootSegment->value()));
  std::cout << "Establishing root finsihed findrootIDDD by id" << std::endl;
  ON_CALL(mockSegments, FindRootID(testing::TypedEq<OmSegment*>(child)))
    .WillByDefault(testing::Return(rootSegment->value()));
  std::cout << "Establishing root finsihed findrootIDDD by pointer" << std::endl;
}

/*
 * Make sure that the children list of the parent now includes this child.
 */
void establishChildren(OmSegment* parent, OmSegment* child, double threshold,
    MockChildren& mockChildren) {
  // TODO since get children returns a reference, we need to store children outside somewhere...
  if (!parent) {
    return;
  }
  child->setParent(parent, threshold);
  std::cout << "getting mock children" << std::endl;
  std::set<OmSegment*> children = mockChildren.GetChildren(parent);
  std::cout << "got children" << std::endl;
  std::cout << "is children emty?" << children.empty() << std::endl;
  if (children.empty()) {
    std::cout << "children is tmepty" << std::endl;
    ON_CALL(mockChildren, GetChildren(testing::TypedEq<om::common::SegID>(parent->value())))
      .WillByDefault(testing::Return(children));
    std::cout << "finish est children by id" << std::endl;
    ON_CALL(mockChildren, GetChildren(testing::TypedEq<const OmSegment*>(parent)))
      .WillByDefault(testing::Return(children));
    std::cout << "finish est children by pointer" << std::endl;
  }
  children.insert(child);

  std::cout << "insertd! " << child->value() << " into " << parent->value() << " now has total: " ;
  for(auto i : children) {
    std::cout << i->value() << " " << std::endl;
  }
  std::cout << std::endl;
  std::cout << "from mock! " << child->value() << " into " << parent->value() << " now has total: ";
  for(auto i : mockChildren.GetChildren(parent)) {
    std::cout << i->value() << " " << std::endl;
  }
  std::cout << std::endl;
}

void connectSegment(OmSegment* parent, OmSegment* child, double threshold,
    MockSegmentsImpl& mockSegments, MockChildren& mockChildren) {
  std::cout << "child has id " << child->value() << std::endl;
  establishRoot(parent, child, mockSegments);
  if (parent) {
    establishChildren(parent, child, threshold, mockChildren);
  }
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

  // prepare test data
  // segmentds with id 3 and 4 will have roots 1 and 2, respectively.
  std::vector<om::segment::Data> data = prepareSegmentData(4);
  std::unique_ptr<OmSegment> segment1 = createSegment(data, 1);
  std::unique_ptr<OmSegment> segment2 = createSegment(data, 2);
  std::unique_ptr<OmSegment> segment1_3 = createSegment(data, 3);
  std::unique_ptr<OmSegment> segment2_4 = createSegment(data, 4);

  // 2 root nodes
  connectSegment(nullptr, segment1.get(), 0, mockSegmentsImpl, mockChildren);
  connectSegment(nullptr, segment2.get(), 0, mockSegmentsImpl, mockChildren);
  connectSegment(segment1.get(), segment1_3.get(), .5, mockSegmentsImpl, mockChildren);
  connectSegment(segment2.get(), segment2_4.get(), .5, mockSegmentsImpl, mockChildren);

std::set<OmSegment*> segsByPointer;
std::set<OmSegment*> segsByID;
  segsByPointer = mockChildren.GetChildren(segment1.get());
  segsByID = mockChildren.GetChildren(segment1.get()->value());
  std::cout << "HERE IS Children by pointer ";
  for(auto i : segsByPointer) {
    std::cout << i->value() << " " << std::endl;
  }
  std::cout << " BLAH BLAH " << std::endl;
  std::cout << "HERE IS Children by ID ";
  for(auto i : segsByID) {
    std::cout << i->value() << " "; 
  }
  std::cout << " BLAH BLAH " << std::endl;
  segsByPointer = mockChildren.GetChildren(segment2.get());
  segsByID = mockChildren.GetChildren(segment2.get()->value());
  std::cout << "HERE IS 2 Children by pointer ";
  for(auto i : segsByPointer) {
    std::cout << i->value() << " " << std::endl;
  }
  std::cout << " BLAH BLAH " << std::endl;
  std::cout << "HERE IS 2 Children by ID ";
  for(auto i : segsByID) {
    std::cout << i->value() << " " << std::endl;
  }
  std::cout << " BLAH BLAH " << std::endl;
  OmSegments mockSegments(segmentation, std::move(mockSegmentsPtr));
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

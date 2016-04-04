#include "precomp.h"
#include "gtest/gtest.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "segment/boostgraph/boostGraph.hpp"
#include "segment/boostgraph/types.hpp"
#include "segment/lowLevel/children.hpp"

namespace test {
namespace segment {

typedef std::vector<std::set<OmSegment*>> ChildrenList;

om::coords::VolumeSystem VOLUME_SYSTEM;
om::common::SegListType SEG_LIST_TYPE = om::common::SegListType::WORKING;

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

class MockSegmentsImpl : public OmSegmentsImpl {
 public:
  MockSegmentsImpl(OmSegmentation* segmentation)
    : OmSegmentsImpl(segmentation) {
    }

  MOCK_CONST_METHOD1(FindRoot, OmSegment*(const om::common::SegID));
  MOCK_CONST_METHOD1(FindRoot, OmSegment*(const OmSegment*));
  MOCK_CONST_METHOD1(FindRootID, om::common::SegID(const om::common::SegID));
  MOCK_CONST_METHOD1(FindRootID, om::common::SegID(const OmSegment*));
};

class MockBoostGraph : public BoostGraph {
 public:
  MockBoostGraph()
    : mockChildren_(om::segment::Children(0)), BoostGraph(mockChildren_) {
    ON_CALL(*this, MinCut(testing::_, testing::_))
      .WillByDefault(testing::Return(std::vector<om::segment::UserEdge>()));
  }

  MOCK_METHOD2(MinCut, std::vector<om::segment::UserEdge>(
        const om::common::SegIDSet sources, const om::common::SegIDSet sinks));
 private:
   om::segment::Children mockChildren_;
};

class MockBoostGraphFactory : public BoostGraphFactory {

 public:
  MockBoostGraphFactory()
    : mockChildren_(om::segment::Children(0)),
      BoostGraphFactory(mockChildren_) {
        std::cout << "boost graph factory dfa ctor" << std::endl;
    ON_CALL(*this, Get(testing::_))
      .WillByDefault(
          testing::Return(std::shared_ptr<BoostGraph>(new MockBoostGraph())));
        std::cout << "boost graph factory dfa ctor done" << std::endl;
  }

  MockBoostGraphFactory(std::shared_ptr<BoostGraph> boostGraph)
  : MockBoostGraphFactory() {
        std::cout << "boost graph factory bg ctor" << std::endl;
    ON_CALL(*this, Get(testing::_)).WillByDefault(testing::Return(boostGraph));
  }
  MOCK_CONST_METHOD1(Get, std::shared_ptr<BoostGraph>(const OmSegment*));

 private:
  om::segment::Children mockChildren_;
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
    std::vector<om::segment::Data>& data) {
  checkIndex(data, segID);
  // offset data by -1 because in this unit test we choose not to express seg 0
  std::unique_ptr<OmSegment> ptr(
      new OmSegment(data[segID - 1], SEG_LIST_TYPE, VOLUME_SYSTEM));
}

void mockSegmentChildren(OmSegment* segment,
    MockChildren& mockChildren,
    ChildrenList& childrenList) {
  // set up the mockchildren call to reference the correct data
  ON_CALL(mockChildren, GetChildren(
        testing::TypedEq<om::common::SegID>(segment->value())))
    .WillByDefault(testing::ReturnRef(childrenList[segment->value() - 1]));
  ON_CALL(mockChildren, GetChildren(
        testing::TypedEq<const OmSegment*>(segment)))
    .WillByDefault(testing::ReturnRef(childrenList[segment->value() - 1]));
}

/*
 * Establish the root in findRoot* functions for the given child.
 * Do this by trying to first getting the parent's root.
 * If the parent doesn't have a root (i.e. no parent),
 * then set the root to itself
 */
void establishRoot(const OmSegment* parent, OmSegment* child,
    MockSegmentsImpl& mockSegmentsImpl) {
  OmSegment* rootSegment = mockSegmentsImpl.FindRoot(parent);

  if (!rootSegment) {
    rootSegment = child;
  }

  //ON_CALL(mockSegmentsImpl, FindRoot(
        //testing::TypedEq<om::common::SegID>(child->value())))
    //.WillByDefault(testing::Return(rootSegment));
  //ON_CALL(mockSegmentsImpl,
      //FindRoot(testing::TypedEq<const OmSegment*>(child)))
    //.WillByDefault(testing::Return(rootSegment));
  //ON_CALL(mockSegmentsImpl, FindRootID(
        //testing::TypedEq<om::common::SegID>(child->value())))
    //.WillByDefault(testing::Return(rootSegment->value()));
  //ON_CALL(mockSegmentsImpl, 
      //FindRootID(testing::TypedEq<const OmSegment*>(child)))
    //.WillByDefault(testing::Return(rootSegment->value()));
}

/*
 * Make sure that the children list of the parent now includes this child.
 */
void addToChildren(OmSegment* parent, OmSegment* child, double threshold) {
  child->setParent(parent, threshold);
}

void addToChildren(OmSegment* parent, OmSegment* child, double threshold,
    MockChildren& mockChildren) {
  addToChildren(parent, child, threshold);
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

} //namespace segment
} //namespace test


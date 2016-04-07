#pragma once
#include "precomp.h"
#include "gmock/gmock.h"
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


/*
 * WARNING: please topologically add nodes using helper functions because
 * because omSegmentsImpl and Children ONLY mock the return values.
 * i.e. add roots first because FindRoot will only update based on previous root
 */
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
  MockBoostGraphFactory(std::shared_ptr<BoostGraph> boostGraph)
    : mockChildren_(om::segment::Children(1)),
      boostGraph_(boostGraph),
      BoostGraphFactory(mockChildren_) {
    ON_CALL(*this, Get(testing::_)).WillByDefault(testing::Return(boostGraph_));
  }

  MOCK_CONST_METHOD1(Get, std::shared_ptr<BoostGraph>(const OmSegment*));

 private:
  om::segment::Children mockChildren_;
  std::shared_ptr<BoostGraph> boostGraph_;
};

/*
 * create segment data and it's children set
 */
std::tuple<std::vector<om::segment::Data>, ChildrenList>
    prepareSegmentData(int numSegments);

/*
 * This void assertion function is only here because google test ONLY supports 
 * asserts within void functions!
 */
void checkIndex(std::vector<om::segment::Data>& data, om::common::SegID segID);

/*
 * return the corresponding children list for the segment
 */
void mockSegmentChildren(OmSegment* segment, MockChildren& mockChildren,
    ChildrenList& childrenList);

/*
 * Create a segment with the corresponding data
 */
std::unique_ptr<OmSegment> createSegment(om::common::SegID segID, 
    std::vector<om::segment::Data>& data);

std::unique_ptr<OmSegment> createSegment(om::common::SegID segID, 
    std::vector<om::segment::Data>& data, MockChildren& mockChildren,
    ChildrenList& childrenList);

/*
 * Establish the root in findRoot* functions for the given child.
 * Do this by trying to first getting the parent's root.
 * If the parent doesn't have a root (i.e. no parent),
 * then set the root to itself
 */
void establishRoot(const OmSegment* parent, OmSegment* child,
    MockSegmentsImpl& mockSegmentsImpl);

/*
 * Make sure that the children list of the parent now includes this child.
 */
void addToChildren(OmSegment* parent, OmSegment* child, double threshold);

void addToChildren(OmSegment* parent, OmSegment* child, double threshold,
    MockChildren& mockChildren);

void connectSegment(OmSegment* parent, OmSegment* child, double threshold, 
    MockSegmentsImpl& mockSegments, MockChildren& mockChildren);

} //namespace segment
} //namespace test


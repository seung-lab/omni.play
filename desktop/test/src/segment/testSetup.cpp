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
#include "segment/testSetup.hpp"

namespace test {
namespace segment {

typedef std::vector<std::set<OmSegment*>> ChildrenList;

om::coords::VolumeSystem VOLUME_SYSTEM;
om::common::SegListType SEG_LIST_TYPE = om::common::SegListType::WORKING;

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
  std::cout << "Segment id " << segment->value() << " has childlist " <<
    &childrenList[segment->value() - 1] << std::endl;
}

/*
 * Create a segment with the corresponding data
 */
std::unique_ptr<OmSegment> createSegment(om::common::SegID segID, 
    std::vector<om::segment::Data>& data) {
  checkIndex(data, segID);
  // offset data by -1 because in this unit test we choose not to express seg 0
  std::unique_ptr<OmSegment> ptr(
      new OmSegment(data[segID - 1], SEG_LIST_TYPE, VOLUME_SYSTEM));
  return ptr;
}

std::unique_ptr<OmSegment> createSegment(om::common::SegID segID, 
    std::vector<om::segment::Data>& data, MockChildren& mockChildren,
    ChildrenList& childrenList) {
  checkIndex(data, segID);
  // offset data by -1 because in this unit test we choose not to express seg 0
  std::unique_ptr<OmSegment> ptr = createSegment(segID, data);
  mockSegmentChildren(ptr.get(), mockChildren, childrenList);
  return ptr;
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

  ON_CALL(mockSegmentsImpl, FindRoot(
        testing::TypedEq<om::common::SegID>(child->value())))
    .WillByDefault(testing::Return(rootSegment));
  ON_CALL(mockSegmentsImpl,
      FindRoot(testing::TypedEq<const OmSegment*>(child)))
    .WillByDefault(testing::Return(rootSegment));
  ON_CALL(mockSegmentsImpl, FindRootID(
        testing::TypedEq<om::common::SegID>(child->value())))
    .WillByDefault(testing::Return(rootSegment->value()));
  ON_CALL(mockSegmentsImpl,
      FindRootID(testing::TypedEq<const OmSegment*>(child)))
    .WillByDefault(testing::Return(rootSegment->value()));
}

/*
 * Make sure that the children list of the parent now includes this child.
 */
void addToChildren(OmSegment* parent, OmSegment* child, double threshold) {
  std::cout << "add to children parent:" << parent->value() <<
    "child " << child->value() << " threshold " << threshold << std::endl;
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
  std::cout << "adding " << child->value() << " to " << parent->value()  << " addr is " << &children << std::endl;
}

void connectSegment(OmSegment* parent, OmSegment* child, double threshold, 
    MockSegmentsImpl& mockSegments, MockChildren& mockChildren) {
  establishRoot(parent, child, mockSegments);
  addToChildren(parent, child, threshold, mockChildren);
}

} //namespace segment
} //namespace test


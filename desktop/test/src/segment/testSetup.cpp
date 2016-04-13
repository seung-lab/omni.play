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
namespace boostgraph {

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

bool setEdge(Vertex vertex1, Vertex vertex2, double newThreshold,
     CapacityProperty& capacityProperty, Graph& graph) {
  Edge edge, edgeReverse;
  bool edgeIsFound, edgeReverseIsFound;
  std::tie(edge, edgeIsFound) = boost::edge(vertex1, vertex2, graph);
  std::tie(edgeReverse, edgeReverseIsFound) =
    boost::edge(vertex2, vertex1, graph);

  if (!edgeIsFound || !edgeReverseIsFound) {
    return false;
  }

  capacityProperty[edge] = newThreshold;
  capacityProperty[edgeReverse] = newThreshold;
  return true;
}

std::vector<std::unique_ptr<OmSegment>> getBasicLineGraph(uint32_t numSegments,
    std::vector<om::segment::Data>& data, MockChildren& mockChildren,
    ChildrenList& childrenList) {

  std::vector<std::unique_ptr<OmSegment>> segments;
  segments.push_back(createSegment(1, data, mockChildren, childrenList));

  for (uint32_t index = 1; index < numSegments; ++index) {
    // since segid 0 does not exist, the segid is essentially 1 + index
    om::common::SegID segmentID = index + 1;
    segments.push_back(createSegment(segmentID, data, mockChildren, childrenList));

    addToChildren(segments[index - 1].get(),
        segments[index].get(), segments[index - 1]->value()/10.0,
        mockChildren);
  }
  return segments;
}

std::vector<std::unique_ptr<OmSegment>> getTrinaryTreeGraph(
    uint32_t numSegments, double defaultThreshold,
    std::vector<om::segment::Data>& data, MockChildren& mockChildren,
    ChildrenList& childrenList) {

  std::vector<std::unique_ptr<OmSegment>> segments;
  segments.push_back(createSegment(1, data, mockChildren, childrenList));

  for (uint32_t index = 1; index < numSegments; ++index) {
    // since segid 0 does not exist, the segid is essentially 1 + index
    om::common::SegID segmentID = index + 1;
    segments.push_back(createSegment(segmentID, data, mockChildren, childrenList));

    addToChildren(segments[(index - 1) / 3].get(),
        segments[index].get(), defaultThreshold,
        mockChildren);
  }
  return segments;
}

const uint32_t BoostGraphTest::DEFAULT_NUM_SEGMENTS = 40;

BoostGraphTest::BoostGraphTest(uint32_t numSegments)
: numSegments_(numSegments), boostGraph_(mockChildren_) {
  std::tie(data_, childrenList_) = prepareSegmentData(numSegments);
}

std::vector<std::unique_ptr<OmSegment>>&
BoostGraphTest::generateBasicLine() {
  tryCreatingSegments([=](){
    return getBasicLineGraph(numSegments_, data_, mockChildren_,
        childrenList_);
  });
  return segments_;
}

std::vector<std::unique_ptr<OmSegment>>& 
BoostGraphTest::BoostGraphTest::generateTrinaryTree(
    double defaultThreshold) {
  tryCreatingSegments([=](){
     return getTrinaryTreeGraph(numSegments_, defaultThreshold,
        data_, mockChildren_, childrenList_);
     });
  return segments_;
}

BoostGraph& BoostGraphTest::GetBoostGraph() {
  return boostGraph_;
}

void BoostGraphTest::tryCreatingSegments(
    std::function<std::vector<std::unique_ptr<OmSegment>>(void)>
    createFunction) {
  if (segments_.size() == 0) {
    segments_ = createFunction();
    boostGraph_.BuildGraph(segments_[0].get());
  } else {
    std::cerr << "Data was already generated previously." <<
     " Will not generate new data" << std::endl;
  }
}

} //namespace boostgraph
} //namespace test


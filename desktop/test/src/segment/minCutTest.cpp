#include "precomp.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "segment/minCut.hpp"
#include "segment/omSegments.h"
#include "segment/omSegmentsImpl.h"
#include "volume/omSegmentation.h"
#include "project/omProject.h"

const std::string fnp("../../test_data/build_project/meshTest.omni");

std::vector<om::segment::Data> prepareData() {
  std::vector<om::segment::Data> data;
  for (int i = 0; i < 5; i++) {
    data.push_back({.value = i});
  }
  return data;
}

class MockSegmentsImpl : public OmSegmentsImpl {
public:
  MockSegmentsImpl(OmSegmentation* segmentation) 
    : OmSegmentsImpl(segmentation) {}

  MOCK_METHOD1(FindRoot, OmSegment*(const om::common::SegID));
};

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

  // set up MockSegmentsImpl and data
  std::unique_ptr<OmSegmentsImpl> 
    mockSegmentsImpl(new MockSegmentsImpl(segmentation));
  
  // prepare test data
  // segmentds with id 3 and 4 will have roots 1 and 2, respectively.
  std::vector<om::segment::Data> data = prepareData();
  om::coords::VolumeSystem volumeSystem;
  om::common::SegListType segListType = om::common::SegListType::WORKING;
  std::unique_ptr<OmSegment> rootSegment1(
      new OmSegment(data[1], segListType, volumeSystem));
  std::unique_ptr<OmSegment> rootSegment2(
      new OmSegment(data[2], segListType, volumeSystem));
  EXPECT_CALL(static_cast<MockSegmentsImpl&>(*mockSegmentsImpl), FindRoot(3))
    .WillRepeatedly(testing::Return(rootSegment1.get()));
  EXPECT_CALL(static_cast<MockSegmentsImpl&>(*mockSegmentsImpl), FindRoot(4))
    .WillRepeatedly(testing::Return(rootSegment2.get()));

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

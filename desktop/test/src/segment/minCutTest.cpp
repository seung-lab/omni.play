#include "precomp.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "segment/minCut.hpp"
#include "segment/omSegments.h"
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

class MockSegments : public OmSegments{
public:
  MockSegments(OmSegmentation* segmentation) : OmSegments(segmentation) {}

  MOCK_METHOD1(FindRoot, OmSegment*(const om::common::SegID));
};

TEST(minCut, testEmpty) {
  // necessary setup for OmSegments
  OmProject::New(QString::fromStdString(fnp));
  SegmentationDataWrapper sdw;
  MockSegments mockSegments(&sdw.Create());

  MinCut minCut(mockSegments);
  om::segment::UserEdge returnEdge = minCut.findEdge(om::common::SegIDSet(),
      om::common::SegIDSet());
  EXPECT_FALSE(returnEdge.valid);
}

TEST(minCut, testNotSameRoot) {
  // necessary setup for OmSegments
  OmProject::New(QString::fromStdString(fnp));
  SegmentationDataWrapper sdw;
  MockSegments mockSegments(&sdw.Create());

  // prepare data to create segments
  std::vector<om::segment::Data> data = prepareData();
  om::coords::VolumeSystem volumeSystem;
  om::common::SegListType segListType = om::common::SegListType::WORKING;

  std::unique_ptr<OmSegment> rootSegment1(new OmSegment(data[1], segListType, volumeSystem));
  std::unique_ptr<OmSegment> rootSegment2(new OmSegment(data[2], segListType, volumeSystem));

  EXPECT_CALL(mockSegments, FindRoot(3)).WillRepeatedly(testing::Return(rootSegment1.get()));
  EXPECT_CALL(mockSegments, FindRoot(4)).WillRepeatedly(testing::Return(rootSegment1.get()));

  om::common::SegIDSet sources;
  om::common::SegIDSet sinks;
  sources.insert(3);
  sinks.insert(4);

  MinCut minCut(mockSegments);

  om::segment::UserEdge returnEdge = minCut.findEdge(sources, sinks);
  EXPECT_FALSE(returnEdge.valid);
}

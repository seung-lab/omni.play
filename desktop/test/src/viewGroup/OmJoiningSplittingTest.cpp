#include "precomp.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <sstream>
#include "viewGroup/omJoiningSplitting.hpp"
#include "gui/tools.hpp"

namespace om {
namespace test {

template <typename T>
std::string SetToString(std::set<T> set) {
  std::ostringstream toReturn;
  toReturn << "[";
  for (T item : set) {
    toReturn << item << ", ";
  }
  toReturn << "]";
  return toReturn.str();
}

const om::common::ID TEST_SEGMENTATION_ID = 1;
/*
 * This is a normal case for join where we try to add values to the first buffer then
 * we want to add values to the second bucket. Check to make sure values
 * were added correctly
 */
TEST(omJoiningSpittingTest , testSeparateBuffers) {
  OmJoiningSplitting joiningSplitting;
  om::common::SegID a = 1, b = 2, c = 3, d = 4;
  joiningSplitting.NextState(om::tool::mode::JOIN);
  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, a));
  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, b));

  joiningSplitting.NextState(om::tool::mode::JOIN);
  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, c));
  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, d));

  const om::common::SegIDSet* firstBuffer = &joiningSplitting.FirstBuffer();
  const om::common::SegIDSet* secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_FALSE(firstBuffer->find(a) == firstBuffer->end());
  EXPECT_TRUE(secondBuffer->find(a) == secondBuffer->end());
  EXPECT_FALSE(firstBuffer->find(b) == firstBuffer->end());
  EXPECT_TRUE(secondBuffer->find(b) == secondBuffer->end());
  EXPECT_TRUE(firstBuffer->find(c) == firstBuffer->end());
  EXPECT_FALSE(secondBuffer->find(c) == secondBuffer->end());
  EXPECT_TRUE(firstBuffer->find(d) == firstBuffer->end());
  EXPECT_FALSE(secondBuffer->find(d) == secondBuffer->end());
}

/*
 * Check to make sure that the buffers are cleared when we reset properly
 */
TEST(omJoiningSpittingTest , testJoinReset) {
  OmJoiningSplitting joiningSplitting;
  om::common::SegID a = 1, b = 2, c = 3, d = 4;
  joiningSplitting.NextState(om::tool::mode::JOIN);
  const om::common::SegIDSet* firstBuffer = &joiningSplitting.FirstBuffer();
  const om::common::SegIDSet* secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->empty());
  EXPECT_TRUE(secondBuffer->empty());
  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, a));
  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, b));

  joiningSplitting.NextState(om::tool::mode::JOIN);
  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, c));
  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, d));

  joiningSplitting.NextState(om::tool::mode::JOIN);
  firstBuffer = &joiningSplitting.FirstBuffer();
  secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->empty());
  EXPECT_TRUE(secondBuffer->empty());
}

/*
 * Check to make sure switching tools resets the buffers
 */
TEST(omJoiningSpittingTest , testResetBuffers) {
  OmJoiningSplitting joiningSplitting;
  om::common::SegID a = 1, b = 2, c = 3, d = 4;
  joiningSplitting.NextState(om::tool::mode::JOIN);
  const om::common::SegIDSet* firstBuffer = &joiningSplitting.FirstBuffer();
  const om::common::SegIDSet* secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->empty());
  EXPECT_TRUE(secondBuffer->empty());
  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, a));
  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, b));

  joiningSplitting.NextState(om::tool::mode::SPLIT);
  firstBuffer = &joiningSplitting.FirstBuffer();
  secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->empty());
  EXPECT_TRUE(secondBuffer->empty());

  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, c));
  joiningSplitting.AddSegment(SegmentDataWrapper(TEST_SEGMENTATION_ID, d));

  joiningSplitting.NextState(om::tool::mode::JOIN);
  firstBuffer = &joiningSplitting.FirstBuffer();
  secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->empty());
  EXPECT_TRUE(secondBuffer->empty());
}

/*
 * Check to make sure that resetting shouldVolumeBeShownBroken is correctly set for join
 */
TEST(omJoiningSpittingTest , testResetShouldVolumeBeShownBrokenJoin) {
  OmJoiningSplitting joiningSplitting;
  joiningSplitting.NextState(om::tool::mode::JOIN);
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());

  joiningSplitting.NextState(om::tool::mode::JOIN);
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());

  joiningSplitting.NextState(om::tool::mode::JOIN);
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * Check to make sure that resetting shouldVolumeBeShownBroken is correctly set for split
 */
TEST(omJoiningSpittingTest , testResetShouldVolumeBeShownBrokenSplit) {
  OmJoiningSplitting joiningSplitting;
  joiningSplitting.NextState(om::tool::mode::SPLIT);
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());

  joiningSplitting.NextState(om::tool::mode::SPLIT);
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());

  joiningSplitting.NextState(om::tool::mode::SPLIT);
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * Check to make sure that resetting shouldVolumeBeShownBroken is correctly set switching 
 * between join and split
 */
TEST(omJoiningSpittingTest , testResetShouldVolumeBeShownBrokenJoinAndSplit) {
  OmJoiningSplitting joiningSplitting;
  joiningSplitting.NextState(om::tool::mode::SPLIT);
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());

  joiningSplitting.NextState(om::tool::mode::JOIN);
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());

  joiningSplitting.NextState(om::tool::mode::SPLIT);
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());

  joiningSplitting.NextState(om::tool::mode::JOIN);
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());
}

}
}

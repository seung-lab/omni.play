#include "precomp.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <sstream>
#include "viewGroup/omJoiningSplitting.hpp"
#include "gui/tools.hpp"

namespace om {
namespace test {
namespace joiningSplitting {

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
const om::common::SegID TEST_SEGMENT_ID_A = 0, TEST_SEGMENT_ID_B = 1,
      TEST_SEGMENT_ID_C = 2, TEST_SEGMENT_ID_D = 3;

/* 
 * Test to make sure the initial state of joiningsplitting is correct
 */
TEST(omJoiningSplittingTest, testInitialValues) {
  OmJoiningSplitting joiningSplitting;
  const om::common::SegIDSet* firstBuffer = &joiningSplitting.FirstBuffer();
  const om::common::SegIDSet* secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->empty());
  EXPECT_TRUE(secondBuffer->empty());
}

/*
 * Add to first buffer correctly
 */
TEST(omJoiningSplittingTest , testFirstBuffer) {
  OmJoiningSplitting joiningSplitting;

  // adding to first buffer
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // test A and B were added to the first buffer
  const om::common::SegIDSet* firstBuffer = &joiningSplitting.FirstBuffer();
  const om::common::SegIDSet* secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_FALSE(firstBuffer->find(TEST_SEGMENT_ID_A) == firstBuffer->end());
  EXPECT_FALSE(firstBuffer->find(TEST_SEGMENT_ID_B) == firstBuffer->end());
  EXPECT_TRUE(secondBuffer->empty());
}

/*
 * Add to second buffer correctly
 */
TEST(omJoiningSplittingTest , testSecondBuffer) {
  OmJoiningSplitting joiningSplitting;

  // adding to first buffer
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  joiningSplitting.PrepareNextState();
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_C));
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_D));

  // test C and D only found in first buffer
  const om::common::SegIDSet* firstBuffer = &joiningSplitting.FirstBuffer();
  const om::common::SegIDSet* secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->find(TEST_SEGMENT_ID_C) == firstBuffer->end());
  EXPECT_TRUE(firstBuffer->find(TEST_SEGMENT_ID_D) == firstBuffer->end());
  EXPECT_FALSE(secondBuffer->find(TEST_SEGMENT_ID_C) == firstBuffer->end());
  EXPECT_FALSE(secondBuffer->find(TEST_SEGMENT_ID_D) == firstBuffer->end());
}

/*
 * Test that when we complete the second step everything is reset correctly
 */
TEST(omJoiningSplittingTest , testReset) {
  OmJoiningSplitting joiningSplitting;
  // add to first buffer
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // advance to second buffer
  joiningSplitting.PrepareNextState();

  // add to second buffer
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_C));
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_D));
  
  // this should reset the buffers
  joiningSplitting.PrepareNextState();

  // test
  const om::common::SegIDSet* firstBuffer = &joiningSplitting.FirstBuffer();
  const om::common::SegIDSet* secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->empty());
  EXPECT_TRUE(secondBuffer->empty());
}

/*
 * Check to make sure switching tools resets the buffers
 */
TEST(omJoiningSplittingTest , testSwitchTools) {
  OmJoiningSplitting joiningSplitting;

  // add to first buffer for joining
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // reset buffer when tool switched to splitting
  joiningSplitting.AddSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_C));
  joiningSplitting.AddSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_D));

  // test to make sure buffers reflect only split segments
  const om::common::SegIDSet* firstBuffer = &joiningSplitting.FirstBuffer();
  const om::common::SegIDSet* secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->find(TEST_SEGMENT_ID_A) == firstBuffer->end());
  EXPECT_TRUE(firstBuffer->find(TEST_SEGMENT_ID_B) == firstBuffer->end());
  EXPECT_FALSE(firstBuffer->find(TEST_SEGMENT_ID_C) == firstBuffer->end());
  EXPECT_FALSE(firstBuffer->find(TEST_SEGMENT_ID_D) == firstBuffer->end());
  EXPECT_TRUE(secondBuffer->empty());
}

/*
 * when joining, we volume should never be shown broken
 */
TEST(omJoiningSplittingTest , testJoinShowBroken) {
  OmJoiningSplitting joiningSplitting;
  // First state with JOIN should be false
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());

  // Second state with JOIN should be false
  joiningSplitting.PrepareNextState();
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * when splitting, we volume should always be shown broken
 */
TEST(omJoiningSplittingTest , testSplitShowBroken) {
  OmJoiningSplitting joiningSplitting;
  // First state with SPLIT should be true
  joiningSplitting.AddSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());

  // Second state with SPLIT should be true
  joiningSplitting.PrepareNextState();
  joiningSplitting.AddSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());
}


/*
 * first state join to split should show broken
 */
TEST(omJoiningSplittingTest , testJoinToSplitShowBrokenFirstState) {
  OmJoiningSplitting joiningSplitting;
  // First state with join
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // Reset state with split should now show broken = true
  joiningSplitting.AddSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * first state split to join should show not broken
 */
TEST(omJoiningSplittingTest , testSplitToJoinShowBrokenFirstState) {
  OmJoiningSplitting joiningSplitting;
  // First state with join
  joiningSplitting.AddSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // Reset state with split should now show broken = false
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * second state split to join should show not broken
 */
TEST(omJoiningSplittingTest , testJoinToSplitShowBrokenSecondState) {
  OmJoiningSplitting joiningSplitting;
  // Second state with join
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  joiningSplitting.PrepareNextState();

  // Reset state with split should now show broken = true
  joiningSplitting.AddSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * second state split to join should show not broken
 */
TEST(omJoiningSplittingTest , testSplitToJoinShowBrokenSecondState) {
  OmJoiningSplitting joiningSplitting;
  // Second state with split
  joiningSplitting.AddSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  joiningSplitting.PrepareNextState();

  // Reset state with join should now show broken = false
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * Tool mode change event does not do anything if the tool is the same
 */
TEST(omJoiningSplittingTest , toolChangeEventSame) {
  OmJoiningSplitting joiningSplitting;
  // first state with split
  joiningSplitting.AddSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // this should have no affect
  joiningSplitting.ToolModeChangeEvent(om::tool::mode::JOIN);

  // first state add some more
  joiningSplitting.AddSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // test A and B were added to the first buffer
  const om::common::SegIDSet* firstBuffer = &joiningSplitting.FirstBuffer();
  const om::common::SegIDSet* secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->find(TEST_SEGMENT_ID_A) == firstBuffer->end());
  EXPECT_FALSE(firstBuffer->find(TEST_SEGMENT_ID_B) == firstBuffer->end());
  EXPECT_TRUE(secondBuffer->empty());
}

/*
 * Tool mode change event does not do anything if the tool is the same
 */
TEST(omJoiningSplittingTest , toolChangeEventDifferent) {
  OmJoiningSplitting joiningSplitting;
  // first state with split
  joiningSplitting.AddSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // this should reset
  joiningSplitting.ToolModeChangeEvent(om::tool::mode::JOIN);

  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // test only B was added to the first buffer
  const om::common::SegIDSet* firstBuffer = &joiningSplitting.FirstBuffer();
  const om::common::SegIDSet* secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->find(TEST_SEGMENT_ID_A) == firstBuffer->end());
  EXPECT_FALSE(firstBuffer->find(TEST_SEGMENT_ID_B) == firstBuffer->end());
  EXPECT_TRUE(secondBuffer->empty());
}

/*
 * start with join then tool mode change to unsupported tool then add to split
 * starts with a clean buffer
 */
TEST(omJoiningSplittingTest , toolChangeUnsupportedReturn) {
  OmJoiningSplitting joiningSplitting;
  // first state with join
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // buffers no longer valid
  joiningSplitting.ToolModeChangeEvent(om::tool::mode::PAN);

  // first state again but now with split
  joiningSplitting.AddSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // test to make sure only segments to split are included
  const om::common::SegIDSet* firstBuffer = &joiningSplitting.FirstBuffer();
  const om::common::SegIDSet* secondBuffer = &joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer->find(TEST_SEGMENT_ID_A) == firstBuffer->end());
  EXPECT_FALSE(firstBuffer->find(TEST_SEGMENT_ID_B) == firstBuffer->end());
  EXPECT_TRUE(secondBuffer->empty());
}

} // namespace joiningSplitting
} // namespace test
} // namespace om

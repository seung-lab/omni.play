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

bool isSegmentInBuffer(om::common::SegID segID, const om::common::SegIDSet& buffer) {
  return buffer.find(segID) != buffer.end();
}

/*
 * Initial state of joiningsplitting is correct
 */
TEST(omJoiningSplittingTest, testInitialValues) {
  OmJoiningSplitting joiningSplitting;
  const om::common::SegIDSet& firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet& secondBuffer = joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer.empty());
  EXPECT_TRUE(secondBuffer.empty());
}

/*
 * select to first buffer correctly
 */
TEST(omJoiningSplittingTest , testFirstBuffer) {
  OmJoiningSplitting joiningSplitting;

  // selecting to first buffer
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // test A and B were selected to the first buffer
  const om::common::SegIDSet& firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet& secondBuffer = joiningSplitting.SecondBuffer();
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_A, firstBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_B, firstBuffer));
  EXPECT_TRUE(secondBuffer.empty());
}

/*
 * select to second buffer correctly
 */
TEST(omJoiningSplittingTest , testSecondBuffer) {
  OmJoiningSplitting joiningSplitting;

  // selecting to first buffer
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  joiningSplitting.GoToNextState();
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_C));
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_D));

  // test C and D only found in first buffer
  const om::common::SegIDSet& firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet& secondBuffer = joiningSplitting.SecondBuffer();
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_A, secondBuffer));
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_C, firstBuffer));
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_D, firstBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_C, secondBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_D, secondBuffer));
}

/*
 * After the second step is completed, nothing is reset and the data 
 * is stil available
 */
TEST(omJoiningSplittingTest , testFinished) {
  OmJoiningSplitting joiningSplitting;
  // select to first buffer
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // advance to second buffer
  joiningSplitting.GoToNextState();

  // select to second buffer
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_C));
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_D));
  
  // this should set the data to be ready for consumption
  joiningSplitting.GoToNextState();

  // test
  const om::common::SegIDSet& firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet& secondBuffer = joiningSplitting.SecondBuffer();
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_A, firstBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_B, firstBuffer));
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_C, firstBuffer));
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_D, firstBuffer));
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_A, secondBuffer));
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_B, secondBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_C, secondBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_D, secondBuffer));
}

/*
 * After second step is completed, preceeding to the next state clears the buffers
 */
TEST(omJoiningSplittingTest , testFinishedRestart) {
  OmJoiningSplitting joiningSplitting;
  // select to first buffer
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // advance to second buffer
  joiningSplitting.GoToNextState();

  // select to second buffer
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_C));
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_D));
  
  // this should set the data to be ready for consumption
  joiningSplitting.GoToNextState();

  // this should reset the buffers as we are back into the first state
  joiningSplitting.GoToNextState();

  // test that buffers are not clear
  const om::common::SegIDSet& firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet& secondBuffer = joiningSplitting.SecondBuffer();
  EXPECT_TRUE(firstBuffer.empty());
  EXPECT_TRUE(secondBuffer.empty());
}

/*
 * Selecting segments after finishing will automatically select to the first buffer again
 */
TEST(omJoiningSplittingTest , testFinishedSelectAgain) {
  OmJoiningSplitting joiningSplitting;
  // select to first buffer
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // advance to second buffer
  joiningSplitting.GoToNextState();

  // select to second buffer
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  
  // this should set the data to be ready for consumption
  joiningSplitting.GoToNextState();

  // this should reset the buffers as we are back into the first state
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_C));
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_D));

  // test C and D are added into the first buffer
  const om::common::SegIDSet firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet secondBuffer = joiningSplitting.SecondBuffer();
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_A, firstBuffer));
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_B, firstBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_C, firstBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_D, firstBuffer));
  EXPECT_TRUE(secondBuffer.empty());
}


/*
 * Switching tools resets the buffers
 */
TEST(omJoiningSplittingTest , testSwitchTools) {
  OmJoiningSplitting joiningSplitting;

  // select to first buffer for joining
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // reset buffer when tool switched to splitting
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_C));
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_D));

  // test to make sure buffers reflect only split segments
  const om::common::SegIDSet firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet secondBuffer = joiningSplitting.SecondBuffer();
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_A, firstBuffer));
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_B, firstBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_C, firstBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_D, firstBuffer));
  EXPECT_TRUE(secondBuffer.empty());
}

/*
 * Is only finished when we've reached the finished state by
 * completing first and second state
 */
TEST(omJoiningSplittingTest , testIsFinished) {
  OmJoiningSplitting joiningSplitting;

  // first state is not finished
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  EXPECT_FALSE(joiningSplitting.IsFinished());
  
  // second state is not finished
  joiningSplitting.GoToNextState();
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_FALSE(joiningSplitting.IsFinished());

  // finished state is finished
  joiningSplitting.GoToNextState();
  EXPECT_TRUE(joiningSplitting.IsFinished());

  // selecting automatically jumps first state which is not finished
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_FALSE(joiningSplitting.IsFinished());
}

/*
 * when joining, we volume should never be shown broken
 */
TEST(omJoiningSplittingTest , testJoinShowBroken) {
  OmJoiningSplitting joiningSplitting;
  // First state with JOIN should be false
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());

  // Second state with JOIN should be false
  joiningSplitting.GoToNextState();
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * when splitting, we volume should always be shown broken
 */
TEST(omJoiningSplittingTest , testSplitShowBroken) {
  OmJoiningSplitting joiningSplitting;
  // First state with SPLIT should be true
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());

  // Second state with SPLIT should be true
  joiningSplitting.GoToNextState();
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());
}


/*
 * first state join to split should show broken
 */
TEST(omJoiningSplittingTest , testJoinToSplitShowBrokenFirstState) {
  OmJoiningSplitting joiningSplitting;
  // First state with join
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // Reset state with split should now show broken = true
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * first state split to join should show not broken
 */
TEST(omJoiningSplittingTest , testSplitToJoinShowBrokenFirstState) {
  OmJoiningSplitting joiningSplitting;
  // First state with join
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // Reset state with split should now show broken = false
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * second state split to join should show not broken
 */
TEST(omJoiningSplittingTest , testJoinToSplitShowBrokenSecondState) {
  OmJoiningSplitting joiningSplitting;
  // Second state with join
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  joiningSplitting.GoToNextState();

  // Reset state with split should now show broken = true
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_TRUE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * second state split to join should show not broken
 */
TEST(omJoiningSplittingTest , testSplitToJoinShowBrokenSecondState) {
  OmJoiningSplitting joiningSplitting;
  // Second state with split
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));
  joiningSplitting.GoToNextState();

  // Reset state with join should now show broken = false
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));
  EXPECT_FALSE(joiningSplitting.ShouldVolumeBeShownBroken());
}

/*
 * Tool mode change event does not do anything if the tool is the same
 */
TEST(omJoiningSplittingTest , toolChangeEventSame) {
  OmJoiningSplitting joiningSplitting;
  // first state with split
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // this should have no affect
  joiningSplitting.ToolModeChangeEvent(om::tool::mode::SPLIT);

  // first state select some more
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // test A and B were selected to the first buffer
  const om::common::SegIDSet& firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet& secondBuffer = joiningSplitting.SecondBuffer();
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_A, firstBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_B, firstBuffer));
  EXPECT_TRUE(secondBuffer.empty());
}

/*
 * Tool mode change event resets when the tool is different
 */
TEST(omJoiningSplittingTest , toolChangeEventDifferent) {
  OmJoiningSplitting joiningSplitting;
  // first state with split
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // this should reset
  joiningSplitting.ToolModeChangeEvent(om::tool::mode::JOIN);

  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // test only B was selected to the first buffer
  const om::common::SegIDSet& firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet& secondBuffer = joiningSplitting.SecondBuffer();
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_A, firstBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_B, firstBuffer));
  EXPECT_TRUE(secondBuffer.empty());
}

/*
 * Tool mode change with different tool event resets even though you select with 
 * original tool
 */
TEST(omJoiningSplittingTest , toolChangeEventDifferentButOriginalTool) {
  OmJoiningSplitting joiningSplitting;
  // first state with split
  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // this should reset
  joiningSplitting.ToolModeChangeEvent(om::tool::mode::JOIN);

  joiningSplitting.SelectSegment(om::tool::mode::SPLIT,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // test only B was selected to the first buffer
  const om::common::SegIDSet& firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet& secondBuffer = joiningSplitting.SecondBuffer();
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_A, firstBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_B, firstBuffer));
  EXPECT_TRUE(secondBuffer.empty());
}

/*
 * start with join then tool mode change to unsupported tool then select to join
 * starts with a clean buffer
 */
TEST(omJoiningSplittingTest , toolChangeUnsupportedReturn) {
  OmJoiningSplitting joiningSplitting;
  // first state with join
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_A));

  // buffers no longer valid
  joiningSplitting.ToolModeChangeEvent(om::tool::mode::PAN);

  // first state again but now with split
  joiningSplitting.SelectSegment(om::tool::mode::JOIN,
      SegmentDataWrapper(TEST_SEGMENTATION_ID, TEST_SEGMENT_ID_B));

  // test to make sure only segments to split (B) are included
  const om::common::SegIDSet& firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet& secondBuffer = joiningSplitting.SecondBuffer();
  EXPECT_FALSE(isSegmentInBuffer(TEST_SEGMENT_ID_A, firstBuffer));
  EXPECT_TRUE(isSegmentInBuffer(TEST_SEGMENT_ID_B, firstBuffer));
  EXPECT_TRUE(secondBuffer.empty());
}

} // namespace joiningSplitting
} // namespace test
} // namespace om

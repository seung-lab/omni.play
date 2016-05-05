#pragma once
#include "precomp.h"

#include "events/listeners.h"
#include "gui/tools.hpp"
#include "utility/dataWrappers.h"
#include "coordinates/coordinates.h"

class OmJoiningSplitting : public om::event::ToolModeEventListener {
 public:
  enum class State { FINISHED_STATE = 0, FIRST_STATE = 1, SECOND_STATE = 2 };

  OmJoiningSplitting();

  const om::common::SegIDSet& FirstBuffer() const;
  const om::common::SegIDSet& SecondBuffer() const;

  void SetShouldVolumeBeShownBroken(const bool shouldVolumeBeShownBroken);
  bool ShouldVolumeBeShownBroken() const;

  // listener for new tool change
  void ToolModeChangeEvent(const om::tool::mode tool) override;

  // Activate the tool (if not activated already) and 
  // Select segment to the current segment buffer.
  void SelectSegment(const om::tool::mode tool,
      const SegmentDataWrapper segmentDataWrapper);

  /*
   * Get the next buffer ready to accept segments.
   * This is a state machine where:
   *
   * FINISHED_STATE --------> FIRST_STATE
   *      ^                        |
   *      |                        |
   *      *----- SECOND_STATE -----* 
   */
  void GoToNextState();

  // are we ready to move to the next state?
  bool IsElementSelected();

  // is the data ready to be read (i.e. first and second states completed)
  bool IsFinished();

  static std::string StateToString(const State state) {
    switch(state) {
      case State::FINISHED_STATE:
        return "FINISHED_STATE";
      case State::FIRST_STATE:
        return "FIRST_STATE";
      case State::SECOND_STATE:
        return "SECOND_STATE";
    }
  }

 private:
  // If tool not already activated, reset the statemachine for the next state
  void activateTool(om::tool::mode tool);

  void reset();

  // clear out first and second buffer and set thet state to FIRST_STATE
  // Also set the volume to be shown broken if we are splitting
  void goToFirstState();
  // clear out second buffer and set thet state to SECOND_STATE
  void goToSecondState();
  // just set the current state to finished state
  void goToFinishedState();

  bool isToolSupported(om::tool::mode tool);

  bool shouldVolumeBeShownBroken_;

  // pointer to the buffer our current state is working on
  om::common::SegIDSet* bufferPointer_;

  // A buffer for each state of join/split we want to save
  om::common::SegIDSet firstBuffer_;
  om::common::SegIDSet secondBuffer_;

  // use this as a block from reactivating the tool
  om::tool::mode currentTool_;

  // current state of this component
  State currentState_;

  // Create a static const set of the supported tools list
  static const std::set<om::tool::mode> JOIN_SPLIT_TOOLS;
};

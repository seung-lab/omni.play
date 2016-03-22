#pragma once
#include "precomp.h"

#include "events/listeners.h"
#include "gui/tools.hpp"
#include "utility/dataWrappers.h"
#include "coordinates/coordinates.h"

class OmJoiningSplitting : public om::event::ToolModeEventListener {
 public:
  enum class State { NOT_INITIALIZED = 0, FIRST_STATE = 1, SECOND_STATE = 2 };

  OmJoiningSplitting();

  const om::common::SegIDSet& FirstBuffer() const;
  const om::common::SegIDSet& SecondBuffer() const;

  void SetShouldVolumeBeShownBroken(const bool shouldVolumeBeShownBroken);
  bool ShouldVolumeBeShownBroken() const;

  // listener for new tool change
  void ToolModeChangeEvent(const om::tool::mode tool) override;

  // Add segment to the current list
  void AddSegment(const SegmentDataWrapper segmentDataWrapper);

  /*
   * Get the next buffer ready to accept segments.
   * This is a state machine where:
   *
   * NOT_INITIALIZED
   *       | NextState()
   *       v               NextState()
   * FIRST_STATE --------------------------> SECOND_STATE
   *  ^   |   ^                                   |
   *  |   |   |     NextState() || tool change    |
   *  *---*   *-----------------------------------*
   * tool change
   *
   */
  void PrepareNextState(const om::tool::mode tool);

  static std::string StateToString(const State state) {
    switch(state) {
      case State::NOT_INITIALIZED:
        return "NOT_INITIALIZED";
      case State::FIRST_STATE:
        return "FIRST_STATE";
      case State::SECOND_STATE:
        return "SECOND_STATE";
    }
  }

 private:
  // Set state to NOT_INITIALIZED if the tool has changed
  void activateTool(om::tool::mode tool);

  // clear out first and second buffer and set thet state to FIRST_STATE
  // Also set the volume to be shown broken if we are splitting
  void prepareFirstState();
  // clear out second buffer and set thet state to SECOND_STATE
  void prepareSecondState();

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
};

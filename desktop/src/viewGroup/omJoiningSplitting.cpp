#pragma once
#include "precomp.h"

#include "events/listeners.h"
#include "utility/dataWrappers.h"
#include "system/omStateManager.h"
#include "gui/tools.hpp"
#include "system/cache/omCacheManager.h"
#include "events/events.h"
#include "viewGroup/omJoiningSplitting.hpp"

OmJoiningSplitting::OmJoiningSplitting() : shouldVolumeBeShownBroken_(false),
  currentState_(State::NOT_INITIALIZED) {}

const om::common::SegIDSet& OmJoiningSplitting::FirstBuffer() const {
  return const_cast<const om::common::SegIDSet&>(firstBuffer_);
}

const om::common::SegIDSet& OmJoiningSplitting::SecondBuffer() const {
  return const_cast<const om::common::SegIDSet&>(secondBuffer_);
}

void OmJoiningSplitting::SetShouldVolumeBeShownBroken(bool shouldVolumeBeShownBroken) {
  shouldVolumeBeShownBroken_ = shouldVolumeBeShownBroken;
}

bool OmJoiningSplitting::ShouldVolumeBeShownBroken() const {
  return shouldVolumeBeShownBroken_;
}

// filter events to only listen for SPLIT and JOIN. should do nothing otherwise 
void OmJoiningSplitting::ToolModeChangeEvent(const om::tool::mode eventTool) {
  activateTool(eventTool);
}


// Activate this tool and notify listeners (buttons)
void OmJoiningSplitting::AddSegment(const SegmentDataWrapper segmentDataWrapper) {
  bufferPointer_->insert(segmentDataWrapper.GetID());
}

void OmJoiningSplitting::activateTool(const om::tool::mode tool) {
  // don't do anything if the tool is the same (i.e. for multithreading events)
  if (currentTool_ == tool) {
    return;
  }

  currentState_ = State::NOT_INITIALIZED;
  currentTool_ = tool;
}

void OmJoiningSplitting::PrepareNextState(const om::tool::mode tool) {
  activateTool(tool);
  switch(currentState_) {
    case State::FIRST_STATE:
      prepareSecondState();
      break;
    case State::NOT_INITIALIZED:
    case State::SECOND_STATE:
    default:
      prepareFirstState();
  }
  bufferPointer_->clear();
}

void OmJoiningSplitting::prepareFirstState() {
  currentState_ = State::FIRST_STATE;
  firstBuffer_.clear();
  secondBuffer_.clear();
  bufferPointer_ = &firstBuffer_;

  switch(currentTool_) {
    case om::tool::mode::SPLIT:
      SetShouldVolumeBeShownBroken(true);
      break;
    case om::tool::mode::JOIN:
    default:
      SetShouldVolumeBeShownBroken(false);
      break;
  }
}

void OmJoiningSplitting::prepareSecondState() {
  currentState_ = State::SECOND_STATE;
  secondBuffer_.clear();
  bufferPointer_ = &secondBuffer_;
}



#pragma once
#include "precomp.h"

#include "events/listeners.h"
#include "utility/dataWrappers.h"
#include "system/omStateManager.h"
#include "gui/tools.hpp"
#include "system/cache/omCacheManager.h"
#include "events/events.h"
#include "viewGroup/omJoiningSplitting.hpp"

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
OmJoiningSplitting::OmJoiningSplitting() : shouldVolumeBeShownBroken_(false),
  currentState_(State::FINISHED_STATE), currentTool_(om::tool::mode::PAN) {}

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
void OmJoiningSplitting::SelectSegment(const om::tool::mode tool,
    const SegmentDataWrapper segmentDataWrapper) {
  activateTool(tool);
  bufferPointer_->insert(segmentDataWrapper.GetID());
  std::cout << "current state" << StateToString(currentState_) << std::endl;
  std::cout << "selected segment : " << segmentDataWrapper.GetID() << std::endl;
  std::cout << "firstBuffer: " << SetToString(firstBuffer_) << std::endl;
  std::cout << "secondBuffer: " << SetToString(secondBuffer_) << std::endl;
}

void OmJoiningSplitting::PrepareNextState() {
  switch(currentState_) {
    case State::FINISHED_STATE:
      prepareFirstState();
      break;
    case State::FIRST_STATE:
      prepareSecondState();
      break;
    case State::SECOND_STATE:
    default:
      prepareFinishedState();
  }
  bufferPointer_->clear();
}

bool OmJoiningSplitting::IsFinished() {
  return currentState_ == State::FINISHED_STATE;
}

void OmJoiningSplitting::activateTool(const om::tool::mode tool) {
  // don't do anything if the tool is the same (i.e. for multithreading events)
  if (currentTool_ == tool) {
    return;
  }

  currentTool_ = tool;

  if (isToolSupported(currentTool_)) {
    reset();
  }
}

void OmJoiningSplitting::reset() {
  currentState_ = State::FINISHED_STATE;
  PrepareNextState();
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

void OmJoiningSplitting::prepareFinishedState() {
  currentState_ = State::FINISHED_STATE;
}

bool OmJoiningSplitting::isToolSupported(om::tool::mode tool) {
  return JOIN_SPLIT_TOOLS.find(tool) != JOIN_SPLIT_TOOLS.end();
}

const om::tool::mode JS_TOOLS[] = {
  om::tool::mode::JOIN, om::tool::mode::SPLIT
};
const std::set<om::tool::mode> OmJoiningSplitting::JOIN_SPLIT_TOOLS(
    JS_TOOLS, JS_TOOLS + sizeof(JS_TOOLS)/sizeof(JS_TOOLS[0]));

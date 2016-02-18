#pragma once
#include "precomp.h"

#include "events/listeners.h"
#include "utility/dataWrappers.h"
#include "system/omStateManager.h"
#include "gui/tools.hpp"
#include "system/cache/omCacheManager.h"
#include "events/events.h"

class OmJoiningSplitting : public om::event::ToolModeEventListener {
 private:
  om::tool::mode currentTool;
  bool showSegmentBrokenOut_;

  boost::optional<om::coords::Global> firstCoordinate_;
  SegmentDataWrapper firstSegment_;

 public:
  OmJoiningSplitting() : showSegmentBrokenOut_(false) {}

  inline bool ShowSegmentBrokenOut() const { return showSegmentBrokenOut_; }

  const SegmentDataWrapper& FirstSegment() const { return firstSegment_; }

  const boost::optional<om::coords::Global>& FirstCoord() const {
    return firstCoordinate_;
  }

  // filter events to only listen for SPLIT and JOIN. should do nothing otherwise 
  void ToolModeChangeEvent() {
    switch(OmStateManager::GetToolMode()) {
      case om::tool::mode::SPLIT:
      case om::tool::mode::JOIN:
        ActivateTool(OmStateManager::GetToolMode());
        break;
      default:
        if (firstCoordinate_) {
          Reset();
        }
    }
  }

  void ActivateTool(om::tool::mode tool) {
    // don't do anything if the tool is the same
    if (currentTool == tool) {
      return;
    }

    // exit previous mode first
    Reset();
    currentTool = tool;

    if (tool == om::tool::mode::SPLIT) {
      showSegmentBrokenOut_ = true;
    }
  }

  // when we listen to the previous tool that was activated, we will reset 
  // the segment and coordinate parameters
  void DeactivateTool() {
    OmStateManager::SetOldToolModeAndSendEvent();
  }

  void Reset() {
    showSegmentBrokenOut_ = false;
    firstCoordinate_.reset();
  }

  // Activate this tool and notify listeners (buttons)
  void SetFirstPoint(om::tool::mode tool, const SegmentDataWrapper& sdw,
                          const om::coords::Global& coord) {
    OmStateManager::SetToolModeAndSendEvent(tool);
    firstSegment_ = sdw;
    firstCoordinate_ = boost::optional<om::coords::Global>(coord);
  }
};

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
  bool shouldVolumeBeShownBroken_;

  boost::optional<om::coords::Global> firstCoordinate_;
  SegmentDataWrapper firstSegment_;

 public:
  OmJoiningSplitting() : shouldVolumeBeShownBroken_(false) {}

  void SetShouldVolumeBeShownBroken(bool shouldVolumeBeShownBroken) { 
    shouldVolumeBeShownBroken_ = shouldVolumeBeShownBroken;

    om::event::Redraw2d();
    om::event::Redraw3d();
  }

  inline bool ShouldVolumeBeShownBroken() const { return shouldVolumeBeShownBroken_; }

  const SegmentDataWrapper& FirstSegment() const { return firstSegment_; }

  const boost::optional<om::coords::Global>& FirstCoord() const {
    return firstCoordinate_;
  }

  // filter events to only listen for SPLIT and JOIN. should do nothing otherwise 
  void ToolModeChangeEvent() {
    om::tool::mode tool = OmStateManager::GetToolMode();

    // don't do anything if the tool is the same
    if (currentTool == tool) {
      return;
    }

    // exit previous mode first
    Reset();

    switch(tool) {
      case om::tool::mode::SPLIT:
        SetShouldVolumeBeShownBroken(true);
        break;
      case om::tool::mode::JOIN:
        break;
    }

    // use this as a block from reactivating the tool
    currentTool = tool;
  }

  // when we listen to the previous tool that was activated, we will reset 
  // the segment and coordinate parameters
  void DeactivateTool() {
    Reset();
    OmStateManager::SetOldToolModeAndSendEvent();
  }

  void Reset() {
    SetShouldVolumeBeShownBroken(false);
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

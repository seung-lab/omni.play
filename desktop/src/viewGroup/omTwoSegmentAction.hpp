#pragma once
#include "precomp.h"

#include "utility/dataWrappers.h"
#include "system/omStateManager.h"
#include "gui/toolbars/toolbarManager.h"
#include "system/cache/omCacheManager.h"
#include "events/events.h"

class OmTwoSegmentAction {
 private:
  bool showSegmentBrokenOut_;
  ToolBarManager* toolBarManager_;

  boost::optional<om::coords::Global> firstCoordinate_;
  SegmentDataWrapper firstSegment_;

 public:
  OmTwoSegmentAction() : showSegmentBrokenOut_(false), toolBarManager_(nullptr) {}

  inline bool ShowSegmentBrokenOut() const { return showSegmentBrokenOut_; }

  const SegmentDataWrapper& FirstSegment() const { return firstSegment_; }

  const boost::optional<om::coords::Global>& FirstCoord() const {
    return firstCoordinate_;
  }

  void SetToolBarManager(ToolBarManager* tbm) { toolBarManager_ = tbm; }

  void Enter(om::common::JoinOrSplit joinOrSplit) {
    showSegmentBrokenOut_ = true;
    om::tool::mode tool;
    if (om::common::JoinOrSplit::JOIN == joinOrSplit) {
      tool = om::common::tool::SPLIT;
    } else {
      tool = om::common::tool::JOIN;
    }

    OmStateManager::SetToolModeAndSendEvent(tool);
    om::event::Redraw3d();
    om::event::Redraw2d();
  }

  void Exit() {
    showSegmentBrokenOut_ = false;
    firstCoordinate_.reset();

    OmStateManager::SetOldToolModeAndSendEvent();
    om::event::Redraw3d();
    om::event::Redraw2d();
  }

  void ExitModeFixButton() {
    toolBarManager_->SetSplittingOff();
    ExitSplitMode();
  }

  void SetFirstPoint(const SegmentDataWrapper& sdw,
                          const om::coords::Global& coord) {
    firstSegment_ = sdw;
    firstCoordinate_ = boost::optional<om::coords::Global>(coord);
  }
};

#pragma once
#include "precomp.h"

#include "utility/dataWrappers.h"
#include "system/omStateManager.h"
#include "gui/toolbars/toolbarManager.h"
#include "gui/tools.hpp"
#include "system/cache/omCacheManager.h"
#include "events/events.h"
#include "common/enums.hpp"

class OmJoiningSplitting {
 private:
  bool showSegmentBrokenOut_;
  ToolBarManager* toolBarManager_;

  boost::optional<om::coords::Global> firstCoordinate_;
  SegmentDataWrapper firstSegment_;

 public:
  OmJoiningSplitting() : showSegmentBrokenOut_(false), toolBarManager_(nullptr) {}

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
      tool = om::tool::mode::JOIN;
    } else {
      tool = om::tool::mode::SPLIT;
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

  void ExitFixButton() {
    toolBarManager_->SetJoiningSplittingOff(om::common::JoinOrSplit::JOIN);
    toolBarManager_->SetJoiningSplittingOff(om::common::JoinOrSplit::SPLIT);
    Exit();
  }

  void SetFirstPoint(const SegmentDataWrapper& sdw,
                          const om::coords::Global& coord) {
    firstSegment_ = sdw;
    firstCoordinate_ = boost::optional<om::coords::Global>(coord);
  }
};

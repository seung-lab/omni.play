#pragma once
#include "precomp.h"

#include "gui/tools.hpp"
#include "common/common.h"
#include "common/logging.h"
#include "events/events.h"
#include "viewGroup/omBrushSize.hpp"
#include "system/omUndoStack.hpp"

class OmStateManagerImpl {
 private:
  om::tool::mode toolModeCur_;
  om::tool::mode toolModePrev_;
  // these tools are temporary and they should not be reactivated.
  std::set<om::tool::mode> transientTools_;

  OmBrushSize brushSize_;
  OmUndoStack undoStack_;

 public:
  OmStateManagerImpl()
      : toolModeCur_(om::tool::PAN), toolModePrev_(om::tool::PAN) {
    transientTools_.insert(om::tool::mode::JOIN);
    transientTools_.insert(om::tool::mode::SPLIT);
    transientTools_.insert(om::tool::mode::SHATTER);
  }

  OmBrushSize* BrushSize() { return &brushSize_; }

  /////////////////////////////////
  ///////          Tool Mode

  inline om::tool::mode GetToolMode() const { return toolModeCur_; }

  inline void SetToolModeAndSendEvent(const om::tool::mode tool) {
    if (tool == toolModeCur_) {
      return;
    }

    // we don't want to be able to return to these tools from SetOldToolModeAndSendEvent()
    if (transientTools_.find(tool) == transientTools_.end()) {
      toolModePrev_ = toolModeCur_;
    }
    toolModeCur_ = tool;

    om::event::ToolChange();
  }

  inline void SetOldToolModeAndSendEvent() {
    // we don't want to be able to return to these tools from SetOldToolModeAndSendEvent()
    if (transientTools_.find(toolModeCur_) == transientTools_.end()) {
      std::swap(toolModePrev_, toolModeCur_);
    } else {
      toolModeCur_ = toolModePrev_;
    }

    om::event::ToolChange();
  }

  /////////////////////////////////
  ///////          UndoStack

  inline OmUndoStack& UndoStack() { return undoStack_; }
};

#pragma once
#include "precomp.h"

#include "gui/tools.hpp"
#include "common/common.h"
#include "common/logging.h"
#include "events/events.h"
#include "viewGroup/omBrushSize.hpp"
#include "system/omUndoStack.hpp"

class OmStateManagerImpl {
 public:
  OmStateManagerImpl() {
    ResetTool();
    transientTools_.insert(om::tool::mode::JOIN);
    transientTools_.insert(om::tool::mode::SPLIT);
    transientTools_.insert(om::tool::mode::SHATTER);
  }

  OmBrushSize* BrushSize() { return &brushSize_; }

  /////////////////////////////////
  ///////          Tool Mode


  inline void ResetTool() {
    toolModeCur_ = toolModePrev_ = om::tool::mode::PAN;
    notifyToolChange();
  }

  inline om::tool::mode GetToolMode() const { return toolModeCur_; }

  inline void SetToolModeAndSendEvent(const om::tool::mode tool) {
    if (tool == toolModeCur_ || !IsEnabled(tool)) {
      return;
    }

    // transient tools can not be returned to!
    if (!IsTransient(toolModeCur_)) {
      toolModePrev_ = toolModeCur_;
    }

    toolModeCur_ = tool;
    notifyToolChange();
  }

  inline void SetOldToolModeAndSendEvent() {
    SetToolModeAndSendEvent(toolModePrev_);
    return;
  }

  inline void EnableTools(const std::set<om::tool::mode> tools, bool isEnabled) {
    for (om::tool::mode tool : tools) {
      enableTool(tool, isEnabled);
    }
    notifyToolChange();
  }

  inline void EnableTool(om::tool::mode tool, bool isEnabled) {
    enableTool(tool, isEnabled);
    notifyToolChange();
  }

  inline bool IsTransient(om::tool::mode tool) {
    // is found in the list of transient tools
    return transientTools_.find(tool) != transientTools_.end();
  }

  inline bool IsEnabled(om::tool::mode tool) {
    // not found in the disabled list
    return disabledTools_.find(tool) == transientTools_.end();
  }

  /////////////////////////////////
  ///////          UndoStack

  inline OmUndoStack& UndoStack() { return undoStack_; }

 private:
  om::tool::mode toolModeCur_;
  om::tool::mode toolModePrev_;
  //
  // these tools are temporary and they should not be reactivated from previous
  std::unordered_set<om::tool::mode, std::hash<int>> transientTools_;

  // these tools are disabled, by default this is empty so all tools are enabled!
  std::unordered_set<om::tool::mode, std::hash<int>> disabledTools_;

  OmBrushSize brushSize_;
  OmUndoStack undoStack_;

  inline void enableTool(om::tool::mode tool, bool isEnabled) {
    if (isEnabled) {
      disabledTools_.erase(tool);
    } else {
      disabledTools_.insert(tool);
    }
  }

  inline void notifyToolChange() {
    om::event::ToolChange(toolModeCur_);
  }
};

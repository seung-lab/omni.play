#pragma once

#include "gui/tools.hpp"
#include "common/common.h"
#include "common/logging.h"
#include "events/events.h"
#include "viewGroup/omBrushSize.hpp"
#include "system/omUndoStack.hpp"

#include <QSize>

class OmStateManagerImpl {
 private:
  om::tool::mode toolModeCur_;
  om::tool::mode toolModePrev_;

  OmBrushSize brushSize_;
  OmUndoStack undoStack_;

 public:
  OmStateManagerImpl()
      : toolModeCur_(om::tool::PAN), toolModePrev_(om::tool::PAN) {}

  OmBrushSize* BrushSize() { return &brushSize_; }

  /////////////////////////////////
  ///////          Tool Mode

  inline om::tool::mode GetToolMode() const { return toolModeCur_; }

  inline void SetToolModeAndSendEvent(const om::tool::mode tool) {
    if (tool == toolModeCur_) {
      return;
    }

    toolModePrev_ = toolModeCur_;
    toolModeCur_ = tool;

    om::event::ToolChange();
  }

  inline void SetOldToolModeAndSendEvent() {
    std::swap(toolModePrev_, toolModeCur_);

    om::event::ToolChange();
  }

  /////////////////////////////////
  ///////          UndoStack

  inline OmUndoStack& UndoStack() { return undoStack_; }
};

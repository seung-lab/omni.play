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

    std::cout << "Setting prev to: " << ttos(toolModePrev_) << " and current to : " <<
      ttos(toolModeCur_) << std::endl;
    om::event::ToolChange();
  }

  inline void SetOldToolModeAndSendEvent() {
    std::cout << "set old tool mode now!" << std::endl;
    std::swap(toolModePrev_, toolModeCur_);

    om::event::ToolChange();
  }

  /////////////////////////////////
  ///////          UndoStack

  inline OmUndoStack& UndoStack() { return undoStack_; }
  std::string ttos(om::tool::mode tool) {
    switch (tool) {
      case om::tool::mode::SPLIT:
        return "SPLIT";
        break;
      case om::tool::mode::JOIN:
        return "JOIN";
        break;
      case om::tool::mode::SHATTER:
        return "SHATTER";
        break;
      default:
        return "OTHER";
    }
  }

};

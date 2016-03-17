#pragma once
#include "precomp.h"

/*
 * Manages data structures that are shared between various parts of the system.
 *
 * Brett Warne - bwarne@mit.edu - 3/14/09
 */

#include "gui/tools.hpp"
#include "common/common.h"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "zi/omUtility.h"

class OmUndoStack;
class OmBrushSize;
class OmStateManagerImpl;

class OmStateManager : private om::singletonBase<OmStateManager> {
 private:
  inline static OmStateManagerImpl& impl() {
    return OmProject::Globals().StateManagerImpl();
  }

 public:
  static OmBrushSize* BrushSize();

  // tool mode
  static void ResetTool();
  static om::tool::mode GetToolMode();
  static void SetToolModeAndSendEvent(const om::tool::mode tool);
  static void SetOldToolModeAndSendEvent();
  static bool IsEnabled(const om::tool::mode tool);
  static void EnableTool(const om::tool::mode tool, bool isEnabled);

  // undostack
  static OmUndoStack& UndoStack();

 private:
  OmStateManager();
  ~OmStateManager();

  friend class zi::singleton<OmStateManager>;
};

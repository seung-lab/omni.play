#pragma once
#include "precomp.h"

#include "utility/dataWrappers.h"
#include "system/omStateManager.h"
#include "gui/toolbars/toolbarManager.h"
#include "system/cache/omCacheManager.h"
#include "events/events.h"

class OmMultiSelecting {
 private:
  bool isSelecting;

 public:
  OmMultiSelecting() : isSelecting(false) {}

  void EnterSelectingMode() {
    isSelecting = true;
  }

  void ExitSelectingMode() {
    isSelecting = false;

  }
};

#pragma once
#include "precomp.h"

#include "utility/dataWrappers.h"
#include "system/omStateManager.h"
#include "gui/toolbars/toolbarManager.h"
#include "system/cache/omCacheManager.h"
#include "segment/omSegmentSelector.h"
#include "events/events.h"
#include "common/logging.h"

class OmMultiSelecting {
 private:
  uint32_t startIndex_;
  std::unique_ptr<OmSegmentSelector> selector_;

 public:
  OmMultiSelecting() : isSelecting_(false) {}

  inline bool IsSelecting() {
    return selector_;
  }

  void EnterSelectingMode(SegmentationDataWrapper& sdw, const std::string& cmt) {
    if (!IsSelecting()) {
      selector_ = std::make_unique(sdw, nullptr, comment);
      startIndex_ = sdw.Segments()->Selection().GetNextOrder();
    }
  }

  void ExitSelectingMode() {
    selector->sendEvent();
    selector.reset();
  }

  void InsertSegments(const om::comon::SegIDSet segIDSet) {
    if(!selector_) {
        log_debugs << "Attempting to multi select but mode is not active" << std::endl;
        return;
    }
    selector_->InsertSegments(segIDSet);
    selector_->UpdateSelectionNow();
  }

  void InsertSegments(const om::comon::SegIDList segIDList) {
    if(!selector_) {
        log_debugs << "Attempting to multi select but mode is not active" << std::endl;
        return;
    }
    selector->InsertSegments(segIDList);
    selector_->UpdateSelectionNow();
  }


};

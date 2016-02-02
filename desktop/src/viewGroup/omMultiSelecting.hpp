#pragma once
#include "precomp.h"

#include <type_traits>
#include "utility/dataWrappers.h"
#include "system/omStateManager.h"
#include "gui/toolbars/toolbarManager.h"
#include "system/cache/omCacheManager.h"
#include "segment/omSegmentSelector.h"
#include "events/events.h"
#include "common/logging.h"

class OmMultiSelecting {
 private:
  std::unique_ptr<OmSegmentSelector> selector_;
  
  // use these two to keep track of the first item that was selected.
  std::unique_ptr<om::common::SegID> firstSelectedSegmentID_;
  std::unique_ptr<uint32_t> firstSelectedSegmentOrder_;

  void createSelectorIfNotExist(SegmentationDataWrapper& sdw, const std::string& cmt)
 public:
  OmMultiSelecting() {}

  inline bool IsSelecting() {
    return selector_;
  }

  void EnterSelectingMode(SegmentationDataWrapper& sdw, const std::string& cmt) {
    if (!IsSelecting()) {
      selector_ = std::make_unique(sdw, nullptr, comment);
    }
  }

  void ExitSelectingMode() {
    selector->sendEvent();
    selector.reset();
    firstSelectedSegmentID_.reset();
    firstSelectedSegmentOrder_.reset();
  }

  void InsertSegments(const om::comon::SegIDSet segIDSet) {
    if (!selector_) {
        log_debugs << "Attempting to multi select insert but mode is not active" << std::endl;
        return;
    }

    selector_->InsertSegments(segIDSet);
    selector_->UpdateSelectionNow();

    if (!firstSelectedSegmentID_) {
      firstSelectedSegmentID_ = *segIDSet.begin();
      firstSelectedSegmentOrder_ = selector_.GetOrderOfAdding(firstSelectedSegmentID_);
    }
  }
};

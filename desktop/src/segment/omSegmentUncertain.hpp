#pragma once

#include "common/common.h"
#include "segment/omSegments.h"
#include "segment/lists/omSegmentLists.h"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/timer.hpp"
#include "volume/omSegmentation.h"

class OmSegmentUncertain {
 public:
  static void SetAsUncertain(
      const SegmentationDataWrapper& sdw,
      std::shared_ptr<std::set<OmSegment*>> selectedSegments,
      const bool uncertain) {
    OmSegmentUncertain uncertainer(sdw, selectedSegments, uncertain);
    uncertainer.setAsUncertain();
  }

 private:
  const SegmentationDataWrapper& sdw_;
  const std::shared_ptr<std::set<OmSegment*>> selectedSegments_;
  const bool uncertain_;

  OmSegmentUncertain(const SegmentationDataWrapper& sdw,
                     std::shared_ptr<std::set<OmSegment*>> selectedSegments,
                     const bool uncertain)
      : sdw_(sdw), selectedSegments_(selectedSegments), uncertain_(uncertain) {}

  void setAsUncertain() {
    static zi::mutex mutex;
    zi::guard g(mutex);

    om::utility::timer timer;

    if (uncertain_) {
      log_debugs(unknown) << "setting " << selectedSegments_->size()
                          << " segments as uncertain...";
    } else {
      log_debugs(unknown) << "setting " << selectedSegments_->size()
                          << " segments as NOT uncertain...";
    }

    for (OmSegment* seg : *selectedSegments_) {
      if (uncertain_) {
        seg->SetListType(om::common::SegListType::UNCERTAIN);
      } else {
        seg->SetListType(om::common::SegListType::WORKING);
      }
    }

    sdw_.SegmentLists()->RefreshGUIlists();

    timer.PrintDone();
  }
};

#pragma once

#include "utility/segmentationDataWrapper.hpp"

class OmJoinSegmentsRunner {
 private:
  SegmentationDataWrapper sdw_;
  om::common::SegIDSet ids_;

 public:
  explicit OmJoinSegmentsRunner(const SegmentationDataWrapper& sdw)
      : sdw_(sdw) {
    if (sdw_.IsSegmentationValid()) {
      ids_ = sdw_.Segments()->Selection().GetSelectedSegmentIDs();
    }
  }

  OmJoinSegmentsRunner(const SegmentationDataWrapper& sdw,
                       const om::common::SegIDSet& ids)
      : sdw_(sdw), ids_(ids) {}

  void Join() { (new OmSegmentJoinAction(sdw_, ids_))->Run(); }
};

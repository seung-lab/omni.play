#pragma once

#include "utility/segmentationDataWrapper.hpp"

class OmJoinSegmentsRunner {
 private:
  SegmentationDataWrapper sdw_;
  OmSegIDsSet ids_;

 public:
  explicit OmJoinSegmentsRunner(const SegmentationDataWrapper& sdw)
      : sdw_(sdw) {
    if (sdw_.IsSegmentationValid()) {
      ids_ = sdw_.Segments()->GetSelectedSegmentIDs();
    }
  }

  OmJoinSegmentsRunner(const SegmentationDataWrapper& sdw,
                       const OmSegIDsSet& ids)
      : sdw_(sdw), ids_(ids) {}

  void Join() { (new OmSegmentJoinAction(sdw_, ids_))->Run(); }
};

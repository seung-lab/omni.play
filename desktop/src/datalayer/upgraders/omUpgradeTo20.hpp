#pragma once
#include "precomp.h"

#include "segment/omSegmentUtils.hpp"
#include "utility/dataWrappers.h"

class OmUpgradeTo20 {
 public:
  void GroupUpValidSegments() {
    SegmentationDataWrapper sdw(1);
    groupUpValidSegments(sdw);
  }

 private:
  void groupUpValidSegments(const SegmentationDataWrapper& sdw) {
    log_infos << "rewriting valid segment data...";
    OmSegmentUtils::ReValidateEveryObject(sdw);
  }
};

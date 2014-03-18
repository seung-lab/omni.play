#pragma once
#include "precomp.h"

#include "datalayer/upgraders/omUpgradeTo14.hpp"
#include "datalayer/upgraders/omUpgradeTo20.hpp"
#include "segment/omSegmentCenter.hpp"

class OmUpgraders {
 public:
  static void RebuildCenterOfSegmentData() {
    FOR_EACH(iter, SegmentationDataWrapper::ValidIDs()) {
      const SegmentationDataWrapper sdw(*iter);
      OmSegmentCenter::RebuildCenterOfSegmentData(sdw);
    }
  }

  static void to14() {
    OmUpgradeTo14 u;
    u.copyDataOutFromHDF5();
  }

  static void to20() {
    OmUpgradeTo20 u;
    u.GroupUpValidSegments();
  }
};

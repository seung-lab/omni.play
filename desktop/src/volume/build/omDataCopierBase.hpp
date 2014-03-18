#pragma once
#include "precomp.h"

#include "common/common.h"
#include "volume/build/omVolumeAllocater.hpp"
#include "volume/omVolumeTypes.hpp"

template <typename VOL>
class OmDataCopierBase {
 private:
  VOL* const vol_;

 public:
  OmDataCopierBase(VOL* vol) : vol_(vol) {}

  void Import() {
    OmTimer timer;

    log_infos << "importing data...";

    doImport();

    timer.PrintDone();
  }

 protected:
  virtual void doImport() = 0;
};

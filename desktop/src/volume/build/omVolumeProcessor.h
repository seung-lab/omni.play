#pragma once
#include "precomp.h"

#include "utility/omTimer.hpp"

class OmChannel;
class OmSegmentation;
class OmAffinityChannel;

class OmVolumeProcessor {
 public:
  OmVolumeProcessor() {}

  template <typename VOL>
  void BuildThreadedVolume(VOL& vol) {
    OmTimer timer;

    try {
      doBuildThreadedVolume(vol);
    }
    catch (...) {
      throw om::IoException("volume build failed");
    }

    log_info("OmVolumeBuilder: BuildThreadedVolume() done : %.6f secs",
             timer.s_elapsed());
  }

 private:
  void doBuildThreadedVolume(OmSegmentation&);
  void doBuildThreadedVolume(OmChannel&);
  void doBuildThreadedVolume(OmAffinityChannel&);
};

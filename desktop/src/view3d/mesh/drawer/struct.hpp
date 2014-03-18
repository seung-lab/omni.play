#pragma once
#include "precomp.h"

namespace om {
namespace v3d {

struct key {
  uint64_t freshness;
  uint64_t dustThreshold;
  bool shouldVolumeBeShownBroken;
  float breakThreshold;

  bool operator<(const key &k) const {
    return std::tie(freshness, dustThreshold, shouldVolumeBeShownBroken,
                    breakThreshold) < std::tie(k.freshness, k.dustThreshold,
                                               k.shouldVolumeBeShownBroken,
                                               k.breakThreshold);
  }
};
}
}  // om::v3d::

#pragma once
#include "precomp.h"

#include "common/common.h"
#include "view3d/omSegmentPointers.h"

namespace om {
namespace v3d {

// segmentation ID, segment ID, mip level, x, y, z,
//   bool shouldVolumeBeShownBroken, float breakThreshold
typedef std::tuple<om::common::ID, om::common::SegID, int, int, int, int, bool,
                   float> MeshSegListKey;

class SegPtrAndColorListValid {
 public:
  SegPtrAndColorListValid() : isValid(false), freshness(0), isFetching(false) {}
  explicit SegPtrAndColorListValid(const bool isFetching)
      : isValid(false), freshness(0), isFetching(isFetching) {}
  SegPtrAndColorListValid(const SegPtrAndColorList& L, const uint32_t f)
      : isValid(true), list(L), freshness(f), isFetching(false) {}

  bool isValid;
  SegPtrAndColorList list;
  uint32_t freshness;
  bool isFetching;
};
}
}  // namespace

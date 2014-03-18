#pragma once
#include "precomp.h"

// temporary wrapper to allow datalayerrefactor View3d

namespace om {
namespace segment {

class Selection {
 private:
  OmSegments* segments_;

 public:
  Selection(OmSegments* s) : segments_(s) {}

  inline const om::common::SegIDSet GetSelectedSegmentIDs() {
    return segments_->GetSelectedSegmentIDs();
  }
};
}
}  // namespace

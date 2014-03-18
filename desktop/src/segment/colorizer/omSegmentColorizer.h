#pragma once
#include "precomp.h"

#include "common/common.h"
#include "segment/colorizer/omSegmentColorizerColorCache.hpp"
#include "segment/colorizer/omSegmentColorizerTypes.h"
#include "utility/omLockedPODs.hpp"

class OmSegment;
class OmSegments;

class OmSegmentColorizer {
 public:
  OmSegmentColorizer(OmSegments*, const om::segment::coloring,
                     const int tileDim, OmViewGroupState* vgs);

  ~OmSegmentColorizer();

  std::shared_ptr<om::common::ColorARGB> ColorTile(
      uint32_t const* const imageData);

  static const std::vector<uint8_t> SelectedColorLookupTable;

 private:
  OmSegmentColorizerColorCache colorCache_;

  SegmentColorParams params_;

  LockedUint64 freshness_;

  void setup();

  friend class SegmentTests1;
};

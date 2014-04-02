#pragma once
#include "precomp.h"

#include "common/common.h"
#include "segment/colorizer/omSegmentColorizerColorCache.hpp"
#include "segment/colorizer/omSegmentColorizerTypes.h"

extern template class std::vector<uint8_t>;

class OmSegment;
class OmSegments;

namespace om {
namespace segment {
class Selection;
}
}

class OmSegmentColorizer {
 public:
  OmSegmentColorizer(OmSegments&, om::segment::coloring, uint32_t tileDim,
                     OmViewGroupState& vgs);

  ~OmSegmentColorizer();

  std::shared_ptr<om::common::ColorARGB> ColorTile(
      uint32_t const* const imageData);

  static const std::vector<uint8_t> SelectedColorLookupTable;

 private:
  OmSegmentColorizerColorCache colorCache_;
  SegmentColorParams params_;
  std::atomic<uint64_t> freshness_;

  void setup();
};

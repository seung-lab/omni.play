#include "segment/colorizer/omSegmentColorizer.h"
#include "segment/colorizer/omSegmentColorizerImpl.hpp"
#include "segment/colorizer/omSegmentColors.hpp"
#include "system/cache/omCacheManager.h"
#include "utility/timer.hpp"
#include "tiles/make_tile.hpp"

extern template class std::vector<uint8_t>;

const std::vector<uint8_t> OmSegmentColorizer::SelectedColorLookupTable =
    OmSegmentColors::makeLookupTable();

OmSegmentColorizer::OmSegmentColorizer(OmSegments& segments,
                                       om::segment::coloring sccType,
                                       uint32_t tileDim, OmViewGroupState& vgs)
    : params_{sccType, tileDim * tileDim, vgs, segments, segments.Selection()} {
  freshness_.store(0);
}

OmSegmentColorizer::~OmSegmentColorizer() {}

void OmSegmentColorizer::setup() {
  // update freshness
  freshness_.store(OmCacheManager::GetFreshness());

  // resize cache, if needed
  const auto curSize = params_.segments.maxValue() + 1;

  if (curSize != colorCache_.Size()) {
    colorCache_.Resize(curSize);
  }
}

std::shared_ptr<om::common::ColorARGB> OmSegmentColorizer::ColorTile(
    uint32_t const* imageData) {
  setup();

  auto ret = om::tile::Make<om::common::ColorARGB>();
  // om::utility::timer timer;

  {
    // prevent vectors from being resized while we're reading
    zi::rwmutex::read_guard g(colorCache_);

    OmSegmentColorizerImpl c(params_, colorCache_, freshness_.load());
    c.ColorTile(imageData, ret.get());
  }

  // timer.PrintV("done coloring tile");

  return ret;
}

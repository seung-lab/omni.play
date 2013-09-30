#include "tiles/pools/omPooledTile.hpp"
#include "segment/colorizer/omSegmentColorizer.h"
#include "segment/colorizer/omSegmentColorizerImpl.hpp"
#include "segment/colorizer/omSegmentColors.hpp"
#include "system/cache/omCacheManager.h"
#include "utility/malloc.hpp"
#include "utility/omTimer.hpp"

const std::vector<uint8_t> OmSegmentColorizer::SelectedColorLookupTable =
    OmSegmentColors::makeLookupTable();

OmSegmentColorizer::OmSegmentColorizer(OmSegments* segments,
                                       const om::segment::coloring sccType,
                                       const int tileDim,
                                       OmViewGroupState* vgs) {
  freshness_.set(0);

  params_.sccType = sccType;
  params_.numElements = tileDim * tileDim;
  params_.vgs = vgs;
  params_.segments = segments;
}

OmSegmentColorizer::~OmSegmentColorizer() {}

void OmSegmentColorizer::setup() {
  // update freshness
  freshness_.set(OmCacheManager::GetFreshness());

  // resize cache, if needed
  const om::common::SegID curSize = params_.segments->getMaxValue() + 1;

  if (curSize != colorCache_.Size()) {
    colorCache_.Resize(curSize);
  }
}

OmPooledTile<om::common::ColorARGB>* OmSegmentColorizer::ColorTile(
    uint32_t const* imageData) {
  setup();

  OmPooledTile<om::common::ColorARGB>* colorMappedDataPtr =
      new OmPooledTile<om::common::ColorARGB>();

  // OmTimer timer;

  {
    //prevent vectors from being resized while we're reading
    zi::rwmutex::read_guard g(colorCache_);

    OmSegmentColorizerImpl c(params_, colorCache_, freshness_.get());
    c.ColorTile(imageData, colorMappedDataPtr->GetData());
  }

  // timer.PrintV("done coloring tile");

  return colorMappedDataPtr;
}

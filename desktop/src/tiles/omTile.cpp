#include "system/cache/omCacheBase.h"
#include "tiles/omChannelTileFilter.hpp"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "utility/dataWrappers.h"
#include "view2d/omView2dConverters.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omMipVolume.h"
#include "chunk/rawChunkSlicer.hpp"
#include "volume/omSegmentation.h"
#include "tile/tileFilters.hpp"
#include "tiles/make_tile.hpp"

OmTile::OmTile(OmCacheBase* cache, const OmTileCoord& key)
    : cache_(cache),
      key_(key),
      coord_(key.getCoord(), key.getViewType(), key.getDepth()),
      tileLength_(key.getVolume().Coords().ChunkDimensions().x),
      mipChunkCoord_(tileToMipCoord()) {}

OmTile::~OmTile() {}

void OmTile::LoadData() {
  if (getVolType() == om::common::CHANNEL) {
    load8bitChannelTile();

  } else {
    load32bitSegmentationTile();
  }
}

struct Slice8bit : public boost::static_visitor<std::shared_ptr<uint8_t>> {
  Slice8bit() {}
  template <typename T>
  std::shared_ptr<uint8_t> operator()(const om::tile::Tile<T>& tile) const {
    om::tile::Filters<T> filter(128);
    auto ret = om::tile::Make<uint8_t>();
    filter.recast(tile.data().get(), ret.get());
    return ret;
  }

  std::shared_ptr<uint8_t> operator()(const om::tile::Tile<uint8_t>& tile)
      const {
    return tile.data();
  }

  std::shared_ptr<uint8_t> operator()(const om::tile::Tile<float>& tile) const {
    om::tile::Filters<float> filter(128);
    auto tileData = om::tile::Make<uint8_t>();
    filter.rescaleAndCast<uint8_t>(tile.data().get(), 0.0, 1.0, 255.0,
                                   tileData.get());

    OmChannelTileFilter::Filter(tileData);
    return tileData;
  }
};

void OmTile::load8bitChannelTile() {
  OmChannel& chan = reinterpret_cast<OmChannel&>(getVol());
  auto tile = chan.GetTile(coord_);
  auto tileData = boost::apply_visitor(Slice8bit(), *tile);
  texture_.reset(new OmTextureID(tileLength_, tileData));
}

void OmTile::load32bitSegmentationTile() {
  OmSegmentation& seg = reinterpret_cast<OmSegmentation&>(getVol());
  auto tile = seg.GetTile(coord_);
  auto rawTile = boost::get<om::tile::Tile<uint32_t>>(tile.get());
  if (!rawTile) {
    log_errors << "Unable to load chunk for slicing.";
    return;
  }
  auto imageData = rawTile->data().get();

  auto colorMappedData =
      key_.getViewGroupState().ColorTile(imageData, tileLength_, key_);

  texture_.reset(new OmTextureID(tileLength_, colorMappedData));
}

om::coords::Chunk OmTile::tileToMipCoord() { return key_.getCoord(); }

int OmTile::getDepth() { return key_.getDepth(); }

om::common::ObjectType OmTile::getVolType() const {
  return getVol().getVolumeType();
}

uint32_t OmTile::NumBytes() const {
  if (getVolType() == om::common::CHANNEL) {
    return 128 * 128;
  }

  return 128 * 128 * 4;
}

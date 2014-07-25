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

OmTile::OmTile(OmCacheBase* cache, const OmTileCoord& key)
    : cache_(cache),
      key_(key),
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
  Slice8bit(int tileLength, om::common::ViewType viewType, int depth)
      : tileLength_(tileLength), viewType_(viewType), depth_(depth) {}
  template <typename T>
  inline std::shared_ptr<T> getRawSlice(T* d) const {
    om::chunk::rawChunkSlicer<T> slicer(128, d);
    return slicer.GetCopyOfTile(viewType_, depth_);
  }

  template <typename T>
  std::shared_ptr<uint8_t> operator()(const om::chunk::Chunk<T>& chunk) const {
    auto rawTile = getRawSlice(chunk.data().get());
    OmTileFilters<T> filter(128);
    return filter.recastToUint8(rawTile.get());
  }

  std::shared_ptr<uint8_t> operator()(const om::chunk::Chunk<uint8_t>& chunk)
      const {
    return getRawSlice(chunk.data().get());
  }

  std::shared_ptr<uint8_t> operator()(const om::chunk::Chunk<float>& chunk)
      const {
    auto rawTile = getRawSlice(chunk.data().get());

    OmTileFilters<float> filter(128);
    auto tileData =
        filter.rescaleAndCast<uint8_t>(rawTile.get(), 0.0, 1.0, 255.0);

    OmChannelTileFilter::Filter(tileData);
    return tileData;
  }

  int tileLength_;
  om::common::ViewType viewType_;
  int depth_;
};

void OmTile::load8bitChannelTile() {
  OmChannel& chan = reinterpret_cast<OmChannel&>(getVol());
  auto chunk = chan.GetChunk(mipChunkCoord_);
  auto tile = boost::apply_visitor(
      Slice8bit(tileLength_, key_.getViewType(), key_.getDepth()), *chunk);
  texture_.reset(new OmTextureID(tileLength_, tile));
}

void OmTile::load32bitSegmentationTile() {
  OmSegmentation& seg = reinterpret_cast<OmSegmentation&>(getVol());
  auto chunk = seg.GetChunk(mipChunkCoord_);
  auto* typedChunk = boost::get<om::chunk::Chunk<uint32_t>>(chunk.get());
  if (!typedChunk) {
    log_errors << "Unable to load chunk for slicing.";
    return;
  }
  om::chunk::rawChunkSlicer<uint32_t> slicer(tileLength_,
                                             typedChunk->data().get());
  auto imageData = slicer.GetCopyOfTile(key_.getViewType(), getDepth());

  auto colorMappedData =
      key_.getViewGroupState().ColorTile(imageData.get(), tileLength_, key_);

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

#include "chunks/omSegChunkDataInterface.hpp"
#include "chunks/omChunkData.hpp"
#include "chunks/omSegChunk.h"
#include "system/cache/omCacheBase.h"
#include "tiles/omChannelTileFilter.hpp"
#include "tiles/omTextureID.h"
#include "tiles/omTile.h"
#include "utility/dataWrappers.h"
#include "view2d/omView2dConverters.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omMipVolume.h"

OmTile::OmTile(OmCacheBase* cache, const OmTileCoord& key)
    : cache_(cache),
      key_(key),
      tileLength_(key.getVolume()->Coords().GetChunkDimension()),
      mipChunkCoord_(tileToMipCoord()) {}

OmTile::~OmTile() {}

void OmTile::LoadData() {
  if (getVolType() == om::common::CHANNEL) {
    load8bitChannelTile();

  } else {
    load32bitSegmentationTile();
  }
}

void OmTile::load8bitChannelTile() {
  OmChannel* chan = reinterpret_cast<OmChannel*>(getVol());
  OmChunk* chunk = chan->GetChunk(mipChunkCoord_);

  auto tileData =
      chunk->Data()->ExtractDataSlice8bit(key_.getViewType(), getDepth());

  OmChannelTileFilter::Filter(tileData);

  texture_.reset(new OmTextureID(tileLength_, tileData));
}

void OmTile::load32bitSegmentationTile() {
  OmSegmentation* seg = reinterpret_cast<OmSegmentation*>(getVol());
  OmSegChunk* chunk = seg->GetChunk(mipChunkCoord_);

  auto imageData =
      chunk->SegData()->ExtractDataSlice32bit(key_.getViewType(), getDepth());

  auto colorMappedData = key_
      .getViewGroupState()->ColorTile(imageData.get(), tileLength_, key_);

  texture_.reset(new OmTextureID(tileLength_, colorMappedData));
}

om::chunkCoord OmTile::tileToMipCoord() { return key_.getCoord(); }

int OmTile::getDepth() { return key_.getDepth(); }

om::common::ObjectType OmTile::getVolType() const {
  return getVol()->getVolumeType();
}

uint32_t OmTile::NumBytes() const {
  if (getVolType() == om::common::CHANNEL) {
    return 128 * 128;
  }

  return 128 * 128 * 4;
}

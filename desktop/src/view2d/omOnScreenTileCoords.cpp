#include "system/cache/omCacheManager.h"
#include "tiles/omTile.h"
#include "view2d/omOnScreenTileCoords.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omViewGroupState.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"
#include "volume/omMipVolume.h"

OmOnScreenTileCoords::OmOnScreenTileCoords(OmView2dState* state,
                                           OmMipVolume& vol)
    : state_(state),
      vol_(vol),
      viewType_(state.getViewType()),
      vgs_(state.getViewGroupState()),
      mipLevel_(state.getMipLevel()),
      tileCoordsAndLocations_(std::make_shared<OmTileCoordsAndLocations>()) {
  freshness_ = 0;
  if (om::common::SEGMENTATION == vol_.getVolumeType()) {
    freshness_ = OmCacheManager::GetFreshness();
  }
}

std::ostream& operator<<(std::ostream& out, const OmOnScreenTileCoords& c) {
  out << c.mipLevel_;
  return out;
}

OmTileCoordsAndLocationsPtr OmOnScreenTileCoords::ComputeCoordsAndLocations() {
  return ComputeCoordsAndLocations(0);
}

OmTileCoordsAndLocationsPtr OmOnScreenTileCoords::ComputeCoordsAndLocations(
    const int depthOffset) {
  if (vol_.IsBuilt()) {
    doComputeCoordsAndLocations(depthOffset);
  }

  OmTileCoordsAndLocationsPtr ret = tileCoordsAndLocations_;
  tileCoordsAndLocations_ = std::make_shared<OmTileCoordsAndLocations>();

  return ret;
}

int numChunks(om::coords::Chunk min, om::coords::Chunk max) {
  return (max.x - min.x + 1) * (max.y - min.y + 1) * (max.z - min.z + 1);
}

void OmOnScreenTileCoords::doComputeCoordsAndLocations(const int depthOffset) {

  om::coords::GlobalBbox bounds = vol_.Coords().Extent();
  om::coords::DataBbox dataBounds = bounds.ToDataBbox(vol_.Coords(), mipLevel_);

  int dataDepth = state_.getViewTypeDepth(
      state_.Location().ToData(vol_.Coords(), mipLevel_));

  // Make sure that we aren't trying to fetch outside of the bounds of the data.
  int targetDepth = dataDepth + depthOffset;

  if (targetDepth < state_.getViewTypeDepth(dataBounds.getMin()) ||
      targetDepth > state_.getViewTypeDepth(dataBounds.getMax())) {
    return;
  }

  // Make sure that the upper left and bottom right don't exceed the volume
  Vector4i viewport = state_.Coords().totalViewport();
  om::coords::Global min = om::coords::Screen(
      viewport.lowerLeftX, viewport.lowerLeftY, state_.Coords()).ToGlobal();
  om::coords::Global max = om::coords::Screen(viewport.width, viewport.height,
                                              state_.Coords()).ToGlobal();

  om::coords::GlobalBbox viewBounds(min, max);

  viewBounds.intersect(bounds);

  if (viewBounds.isEmpty()) {
    return;
  }

  om::coords::Chunk minChunk =
      om::coords::Global(viewBounds.getMin()).ToChunk(vol_.Coords(), mipLevel_);
  om::coords::Chunk maxChunk =
      om::coords::Global(viewBounds.getMax()).ToChunk(vol_.Coords(), mipLevel_);

  // iterate over all chunks on the screen
  for (int x = minChunk.x; x <= maxChunk.x; x++) {
    for (int y = minChunk.y; y <= maxChunk.y; y++) {
      for (int z = minChunk.z; z <= maxChunk.z; z++) {
        om::coords::Chunk coord(mipLevel_, x, y, z);
        computeTile(coord, depthOffset);
      }
    }
  }
}

void OmOnScreenTileCoords::computeTile(const om::coords::Chunk& chunkCoord,
                                       const int depthOffset) {
  if (!vol_.Coords().ContainsMipChunk(chunkCoord)) {
    return;  // Rounding errors can cause bad chunk coords to slip through.
  }

  if (depthOffset)  // i.e. if we are pre-fetching
  {
    if (om::common::CHANNEL == vol_.getVolumeType()) {
      OmChannel* chan = reinterpret_cast<OmChannel*>(vol_);

      const std::vector<OmFilter2d*> filters = chan->GetFilters();

      FOR_EACH(iter, filters) {
        makeTileCoordFromFilter(*iter, chunkCoord, depthOffset);
      }
    }
  }

  OmTileCoordAndVertices pair = {
      makeTileCoord(chunkCoord, depthOffset, vol_, freshness_),
      computeVertices(chunkCoord, vol_)};

  // log_infos << chunkCoord << ":" << depthOffset << " - " << pair.tileCoord <<
  // std::endl;
  tileCoordsAndLocations_->push_back(pair);
}

void OmOnScreenTileCoords::makeTileCoordFromFilter(
    OmFilter2d* filter, const om::coords::Chunk& chunkCoord,
    const int depthOffset) {
  const om::FilterType filterType = filter->FilterType();

  OmMipVolume* vol = nullptr;
  int freshness = 0;

  if (om::OVERLAY_NONE == filterType) {
    return;

  } else if (om::OVERLAY_CHANNEL == filterType) {
    vol = filter->GetChannel();

  } else {
    vol = filter->GetSegmentation();
    freshness = OmCacheManager::GetFreshness();
  }

  OmTileCoordAndVertices pair = {
      makeTileCoord(chunkCoord, depthOffset, vol, freshness),
      computeVertices(chunkCoord, vol)};

  tileCoordsAndLocations_->push_back(pair);
}

OmTileCoord OmOnScreenTileCoords::makeTileCoord(const om::coords::Chunk& coord,
                                                const int depthOffset,
                                                OmMipVolume& vol,
                                                int freshness) {
  om::coords::Data loc = state_.Location().ToData(vol.Coords(), mipLevel_);
  int targetDepth = state_.getViewTypeDepth(loc) + depthOffset;
  state_.setViewTypeDepth(loc, targetDepth);

  return OmTileCoord(coord, state_.getViewType(), loc.ToTileDepth(viewType_),
                     vol, freshness, vgs_, state_.getObjectType());
}

GLfloatBox OmOnScreenTileCoords::computeVertices(const om::coords::Chunk& coord,
                                                 const OmMipVolume& vol) {
  auto bounds = coord.BoundingBox(vol.Coords());
  auto min = bounds.getMin().ToGlobal().ToScreen(state_.Coords());
  auto max = bounds.getMax().ToGlobal().ToScreen(state_.Coords());

  GLfloatBox glBox;
  glBox.lowerLeft.y = min.y;
  glBox.upperRight.y = max.y;
  glBox.lowerLeft.x = min.x;
  glBox.upperRight.x = max.x;

  glBox.lowerRight.x = glBox.upperRight.x;
  glBox.lowerRight.y = glBox.lowerLeft.y;

  glBox.upperLeft.x = glBox.lowerLeft.x;
  glBox.upperLeft.y = glBox.upperRight.y;

  return glBox;
}

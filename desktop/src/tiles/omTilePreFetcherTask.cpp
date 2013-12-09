#include "tiles/omTilePreFetcherTask.hpp"
#include "view2d/omOnScreenTileCoords.h"
#include "view2d/omView2dState.hpp"
#include "tiles/cache/omTileCache.h"

// make a shallow copy of state information to avoid any locking issues
//  with mTotalViewport
OmTilePreFetcherTask::OmTilePreFetcherTask(OmView2dState* state)
    : state_(new OmView2dState(*state)) {}

void OmTilePreFetcherTask::run() {
  onScreenTileCoords_.reset(
      new OmOnScreenTileCoords(state_.get(), state_->getVol()));

  const int maxPrefetchDepth = 32;

  for (int i = 1; i < maxPrefetchDepth; ++i) {
    if (shouldExitEarly()) {
      return;
    }
    preLoadDepth(i);

    if (shouldExitEarly()) {
      return;
    }
    preLoadDepth(-i);
  }
}

void OmTilePreFetcherTask::preLoadDepth(const int depthOffset) {
  OmTileCoordsAndLocationsPtr tilesCoordsToFetch =
      onScreenTileCoords_->ComputeCoordsAndLocations(depthOffset);

  int count = 0;
  FOR_EACH(tileCL, *tilesCoordsToFetch) {
    if (shouldExitEarly()) {
      return;
    }

    ++count;

    OmTileCache::Prefetch(tileCL->tileCoord, depthOffset);
  }
}

bool OmTilePreFetcherTask::shouldExitEarly() {
  return OmTileCache::AreDrawersActive();
}

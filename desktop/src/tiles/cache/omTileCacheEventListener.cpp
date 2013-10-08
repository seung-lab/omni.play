#include "tiles/cache/omTileCacheEventListener.hpp"
#include "tiles/cache/omTileCacheImpl.hpp"

OmTileCacheEventListener::OmTileCacheEventListener(OmTileCacheImpl* cache)
    : cache_(cache) {}

void OmTileCacheEventListener::ViewCenterChangeEvent() {
  cache_->ClearFetchQueues();
  OmEvents::Redraw2d();
}

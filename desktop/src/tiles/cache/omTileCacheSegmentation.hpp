#pragma once

#include "tiles/cache/omThreadedTileCache.h"
#include "tiles/omTile.h"
#include "coordinates/tile.h"
#include "utility/lockedObjects.hpp"

class OmSegmentation;

class OmTileCacheSegmentation : public OmThreadedTileCache {
 public:
  OmTileCacheSegmentation()
      : OmThreadedTileCache("Segmentation Tiles", 128 * 128 * 4) {}

  virtual ~OmTileCacheSegmentation() {}

  void Load(OmSegmentation*) {}

  friend class OmCacheManager;
};

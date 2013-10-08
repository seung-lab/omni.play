#pragma once

#include "common/om.hpp"
#include "tiles/cache/omThreadedTileCache.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "utility/omLockedObjects.h"

class OmSegmentation;

class OmTileCacheSegmentation : public OmThreadedTileCache {
 public:
  OmTileCacheSegmentation()
      : OmThreadedTileCache("Segmentation Tiles", 128 * 128 * 4) {}

  virtual ~OmTileCacheSegmentation() {}

  void Load(OmSegmentation*) {}

  friend class OmCacheManager;
};

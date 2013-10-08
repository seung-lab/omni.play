#pragma once

#include "common/om.hpp"
#include "tiles/cache/omThreadedTileCache.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTileTypes.hpp"

class OmChannelImpl;

class OmTileCacheChannel : public OmThreadedTileCache {
 public:
  OmTileCacheChannel() : OmThreadedTileCache("Channel Tiles", 128 * 128) {}

  virtual ~OmTileCacheChannel() {}

  void Load(OmChannelImpl*) {}
};

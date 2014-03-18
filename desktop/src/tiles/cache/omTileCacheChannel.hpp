#pragma once
#include "precomp.h"

#include "tiles/cache/omThreadedTileCache.h"
#include "tiles/omTile.h"
#include "coordinates/tile.h"
#include "tiles/omTileTypes.hpp"

class OmChannelImpl;

class OmTileCacheChannel : public OmThreadedTileCache {
 public:
  OmTileCacheChannel() : OmThreadedTileCache("Channel Tiles", 128 * 128) {}

  virtual ~OmTileCacheChannel() {}

  void Load(OmChannelImpl*) {}
};

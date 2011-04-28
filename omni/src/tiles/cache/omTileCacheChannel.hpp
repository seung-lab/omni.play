#ifndef OM_TILE_CACHE_CHANNEL_HPP
#define OM_TILE_CACHE_CHANNEL_HPP

#include "common/om.hpp"
#include "tiles/cache/omThreadedTileCache.h"
#include "tiles/omTile.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTileTypes.hpp"

class OmTileCacheChannel : public OmThreadedTileCache {
public:
    OmTileCacheChannel()
        : OmThreadedTileCache("Channel Tiles")
    {}

private:
    OmTilePtr HandleCacheMiss(const OmTileCoord& key)
    {
        OmTile* tile = new OmTile(this, key);
        tile->LoadData();
        return OmTilePtr(tile);
    }
};

#endif

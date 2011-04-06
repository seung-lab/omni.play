#ifndef OM_TILE_CACHE_CHANNEL_HPP
#define OM_TILE_CACHE_CHANNEL_HPP

#include "common/om.hpp"
#include "system/cache/omThreadedCache.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTile.h"

class OmTileCacheChannel : public OmThreadedCache<OmTileCoord, OmTilePtr> {
public:
    OmTileCacheChannel()
        : OmThreadedCache<OmTileCoord, OmTilePtr>(om::TILE_CACHE,
                                                  "Channel Tiles",
                                                  om::DONT_THROTTLE)
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

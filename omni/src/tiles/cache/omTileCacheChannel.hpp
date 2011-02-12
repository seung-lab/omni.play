#ifndef OM_TILE_CACHE_CHANNEL_HPP
#define OM_TILE_CACHE_CHANNEL_HPP

#include "common/om.hpp"
#include "system/cache/omThreadedCache.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTile.h"

class OmTileCacheChannel : public OmThreadedCache<OmTileCoord, OmTilePtr> {
private:
	static const int NUM_THREADS = 3;

public:
	OmTileCacheChannel()
		: OmThreadedCache<OmTileCoord, OmTilePtr>(VRAM_CACHE_GROUP,
												  "Channel Tiles",
												  NUM_THREADS,
												  om::THROTTLE)
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

#ifndef OM_TILE_CACHE_IMPL_H
#define OM_TILE_CACHE_IMPL_H

#include "common/om.hpp"
#include "system/cache/omThreadedCache.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTile.h"
#include "utility/omLockedObjects.h"

class OmTileCacheChannel : public OmThreadedCache<OmTileCoord, OmTilePtr> {
public:
	OmTileCacheChannel()
		: OmThreadedCache<OmTileCoord, OmTilePtr>(VRAM_CACHE_GROUP,
							  "Channel Tiles")
	{}

private:
	OmTilePtr HandleCacheMiss(const OmTileCoord& key){
		OmTile* tile = new OmTile(this, key);
		tile->loadData();
		return OmTilePtr(tile);
	}
};

class OmTileCacheNonChannel : private OmThreadedCache<OmTileCoord, OmTilePtr> {
public:
	OmTileCacheNonChannel()
		: OmThreadedCache<OmTileCoord, OmTilePtr>(VRAM_CACHE_GROUP,
							  "Non-channel Tiles")
	{}

	void Get(OmTilePtr&, const OmTileCoord&, const OM::BlockingRead);
	void RemoveSpaceCoord(const SpaceCoord& coord);

private:
	LockedMultiMap<SpaceCoord, OmTileCoord> keysBySpaceCoord_;
	zi::Mutex mutex_;
	LockedUint64 freshness_;

	bool isKeyFresh(const OmTileCoord& key);
	OmTilePtr HandleCacheMiss(const OmTileCoord& key);

	friend class OmCacheManager;
};

#endif

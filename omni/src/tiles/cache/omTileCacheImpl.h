#ifndef OM_TILE_CACHE_IMPL_H
#define OM_TILE_CACHE_IMPL_H

#include "common/om.hpp"
#include "system/cache/omThreadedCache.h"
#include "utility/omLockedObjects.h"

class OmTileCacheChannel : public OmThreadedCache<OmTileCoord, OmTilePtr> {
public:
	OmTileCacheChannel();

private:
	OmTilePtr HandleCacheMiss(const OmTileCoord& key);
};

class OmTileCacheNonChannel : private OmThreadedCache<OmTileCoord, OmTilePtr> {
public:
	OmTileCacheNonChannel();

	void Get(OmTilePtr&, const OmTileCoord&, const om::BlockingRead);
	void RemoveSpaceCoord(const SpaceCoord& coord);

private:
	LockedMultiMap<SpaceCoord, OmTileCoord> keysBySpaceCoord_;
	zi::mutex mutex_;
	LockedUint64 freshness_;

	bool isKeyFresh(const OmTileCoord& key);
	OmTilePtr HandleCacheMiss(const OmTileCoord& key);

	friend class OmCacheManager;
};

#endif

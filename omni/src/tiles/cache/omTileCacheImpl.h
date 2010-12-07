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

	void Get(OmTilePtr&, const OmTileCoord&, const om::Blocking);
	void RemoveSpaceCoord(const SpaceCoord& coord);
	void Clear();
	void Prefetch(const OmTileCoord& key);

private:
	LockedMultiMap<SpaceCoord, OmTileCoord> keysBySpaceCoord_;
	zi::mutex mutex_;
	LockedUint64 freshness_;

	bool isKeyFresh(const OmTileCoord& key);
	OmTilePtr HandleCacheMiss(const OmTileCoord& key);

	friend class OmCacheManager;
};

#endif

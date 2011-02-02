#ifndef OM_TILE_CACHE_SEGMENTATION_HPP
#define OM_TILE_CACHE_SEGMENTATION_HPP

#include "common/om.hpp"
#include "system/cache/omThreadedCache.h"
#include "utility/omLockedObjects.h"
#include "tiles/omTileCoord.h"
#include "tiles/omTile.h"

class OmTileCacheSegmentation : private OmThreadedCache<OmTileCoord, OmTilePtr>{
private:
	static const int NUM_THREADS = 3;

public:
	OmTileCacheSegmentation()
		: OmThreadedCache<OmTileCoord, OmTilePtr>(VRAM_CACHE_GROUP,
												  "Segmentation Tiles",
												  NUM_THREADS,
												  om::THROTTLE)
	{}

	void Get(OmTilePtr& ptr, const OmTileCoord& key,
			 const om::Blocking isBlocking)
	{
		zi::guard g(mutex_);

		if(key.getFreshness() > freshness_.get()){
			InvalidateCache();
			keysByDataCoord_.clear();
			freshness_.set(key.getFreshness());
		}

		OmThreadedCache<OmTileCoord, OmTilePtr>::Get(ptr, key, isBlocking);
	}

	void RemoveDataCoord(const DataCoord& coord)
	{
		boost::shared_ptr<std::list<OmTileCoord> > tileCoordsToRemove =
			keysByDataCoord_.removeKey(coord);

		FOR_EACH(iter, *tileCoordsToRemove){
			Remove(*iter);
		}
	}

	void Clear(){
		OmThreadedCache<OmTileCoord, OmTilePtr>::Clear();
	}

	void Prefetch(const OmTileCoord& key){
		OmThreadedCache<OmTileCoord, OmTilePtr>::Prefetch(key);
	}

private:
	zi::mutex mutex_;
	LockedMultiMap<DataCoord, OmTileCoord> keysByDataCoord_;
	LockedUint64 freshness_;

	bool isKeyFresh(const OmTileCoord& key)
	{
		if(key.getFreshness() < freshness_.get()){
			return false; //old tile request
		}

		return true;
	}

	OmTilePtr HandleCacheMiss(const OmTileCoord& key)
	{
		if(!isKeyFresh(key)){
			return OmTilePtr();
		}

		keysByDataCoord_.insert(key.getDataCoord(), key);

		OmTile* tile = new OmTile(this, key);
		tile->LoadData();
		return OmTilePtr(tile);
	}

	friend class OmCacheManager;
};

#endif

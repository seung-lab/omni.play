#include "tiles/cache/omTileCacheImpl.h"

void OmTileCacheNonChannel::RemoveSpaceCoord(const SpaceCoord& coord)
{
	boost::shared_ptr<std::list<OmTileCoord> >
		tileCoordsToRemove = keysBySpaceCoord_.removeKey(coord);

	FOR_EACH(iter, *tileCoordsToRemove){
		Remove(*iter);
	}
}

OmTilePtr OmTileCacheNonChannel::HandleCacheMiss(const OmTileCoord& key)
{
	if(!isKeyFresh(key)){
		return OmTilePtr();
	}

	keysBySpaceCoord_.insert(key.getSpaceCoord(), key);

	OmTile* tile = new OmTile(this, key);
	tile->loadData();
	return OmTilePtr(tile);
}

bool OmTileCacheNonChannel::isKeyFresh(const OmTileCoord& key)
{
	if(key.getFreshness() < freshness_.get()){
		return false; //old tile request
	}

	return true;
}

void OmTileCacheNonChannel::Get(OmTilePtr& ptr, const OmTileCoord& key,
				const OM::BlockingRead isBlocking)
{
	zi::Guard g(mutex_);
	if(key.getFreshness() > freshness_.get()){
		InvalidateCache();
		keysBySpaceCoord_.clear();
		freshness_.set(key.getFreshness());
	}

	OmThreadedCache<OmTileCoord, OmTilePtr>::Get(ptr, key, isBlocking);
}

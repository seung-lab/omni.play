#ifndef OM_TILE_CACHE_H
#define OM_TILE_CACHE_H

#include "system/cache/omThreadedCache.h"
#include "view2d/omTileCoord.h"
#include "view2d/omTextureID.h"

class OmThreadedCachingTile;

class OmTileCache : public OmThreadedCache<OmTileCoord, OmTextureIDPtr> {
public:
	OmTileCache(OmThreadedCachingTile * parent);
	
	OmTextureIDPtr HandleCacheMiss(const OmTileCoord &key);

private:
	OmThreadedCachingTile *const mOmThreadedCachingTile;
};

#endif

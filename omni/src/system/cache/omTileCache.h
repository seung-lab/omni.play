#ifndef OM_TILE_CACHE_H
#define OM_TILE_CACHE_H

#include "system/cache/omThreadedCache.h"
#include "view2d/omTileCoord.h"
#include "view2d/omTextureID.h"

class OmThreadedCachingTile;

class OmTileCache : public OmThreadedCache<OmTileCoord, OmTextureID> {
public:
	OmTileCache(OmThreadedCachingTile * parent);
	
	OmTextureID* HandleCacheMiss(const OmTileCoord &key);

private:
	OmThreadedCachingTile *const mOmThreadedCachingTile;
};

#endif

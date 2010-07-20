#include "system/cache/omTileCache.h"
#include "view2d/omThreadedCachingTile.h"

OmTileCache::OmTileCache(OmThreadedCachingTile * parent) 
	: OmThreadedCache<OmTileCoord, OmTextureID>(VRAM_CACHE_GROUP)
	, mOmThreadedCachingTile(parent)
{
}
	
OmTextureID* OmTileCache::HandleCacheMiss(const OmTileCoord & key)
{
	//return mesh to cache
	return mOmThreadedCachingTile->BindToTextureID(key, this);
}

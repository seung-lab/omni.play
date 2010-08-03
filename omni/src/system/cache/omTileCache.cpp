#include "system/cache/omTileCache.h"
#include "view2d/omThreadedCachingTile.h"

OmTileCache::OmTileCache(OmThreadedCachingTile * parent) 
	: OmThreadedCache<OmTileCoord, OmTextureIDPtr>(VRAM_CACHE_GROUP)
	, mOmThreadedCachingTile(parent)
{
}
	
OmTextureIDPtr OmTileCache::HandleCacheMiss(const OmTileCoord & key)
{
	return mOmThreadedCachingTile->BindToTextureID(key, this);
}

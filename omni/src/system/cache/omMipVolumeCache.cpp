#include "volume/omMipVolume.h"
#include "system/cache/omMipVolumeCache.h"

OmMipVolumeCache::OmMipVolumeCache(OmMipVolume * parent) 
	: OmThreadedCache<OmMipChunkCoord, OmMipChunk>(RAM_CACHE_GROUP)
	, mMipVolume(parent)
{
}
	
OmMipChunk* OmMipVolumeCache::HandleCacheMiss(const OmMipChunkCoord &rMipCoord)
{
	assert(mMipVolume->ContainsMipChunkCoord(rMipCoord));
	return new OmMipChunk(rMipCoord, mMipVolume);
}

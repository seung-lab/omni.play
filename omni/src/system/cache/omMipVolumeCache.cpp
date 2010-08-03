#include "volume/omMipVolume.h"
#include "system/cache/omMipVolumeCache.h"

OmMipVolumeCache::OmMipVolumeCache(OmMipVolume * parent)
	: OmThreadedCache<OmMipChunkCoord, OmMipChunkPtr>(RAM_CACHE_GROUP)
	, mMipVolume(parent)
{
}

OmMipChunkPtr OmMipVolumeCache::HandleCacheMiss(const OmMipChunkCoord &rMipCoord)
{
	assert(mMipVolume->ContainsMipChunkCoord(rMipCoord));
	return OmMipChunkPtr(new OmMipChunk(rMipCoord, mMipVolume));
}

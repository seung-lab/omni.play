#include "volume/omMipVolume.h"
#include "volume/omMipChunk.h"
#include "volume/omThreadChunkLevel.h"
#include "volume/omVolume.h"
#include "system/cache/omThreadChunkThreadedCache.h"

OmThreadChunkThreadedCache::OmThreadChunkThreadedCache(OmMipVolume * volume)
	: OmThreadedCache<OmMipChunkCoord,
			  OmThreadChunkLevelPtr>(RAM_CACHE_GROUP,
						 "Downsampling")
	, mMipVolume(volume)
{
}

OmThreadChunkThreadedCache::~OmThreadChunkThreadedCache()
{
}

void OmThreadChunkThreadedCache::GetChunk(OmThreadChunkLevelPtr& p_value,
					  const OmMipChunkCoord& key,
					  bool block)
{
	Get(p_value, key, block);
}

OmThreadChunkLevelPtr
OmThreadChunkThreadedCache::HandleCacheMiss(const OmMipChunkCoord &key){
	assert(mMipVolume->ContainsThreadChunkCoord(key));

	return OmThreadChunkLevelPtr(new OmThreadChunkLevel(key, mMipVolume));
}

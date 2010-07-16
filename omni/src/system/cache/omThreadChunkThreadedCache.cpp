#include "volume/omMipVolume.h"
#include "volume/omMipChunk.h"
#include "volume/omThreadChunkLevel.h"
#include "volume/omVolume.h"
#include "system/cache/omThreadChunkThreadedCache.h"

OmThreadChunkThreadedCache::OmThreadChunkThreadedCache(OmMipVolume * volume) 
	: OmThreadedCache<OmMipChunkCoord, OmThreadChunkLevel>(RAM_CACHE_GROUP)
	, mMipVolume(volume)
{
        int chunkDim = volume->GetThreadChunkDimension();
        SetObjectSize(chunkDim*chunkDim*chunkDim*volume->GetBytesPerSample());
}

OmThreadChunkThreadedCache::~OmThreadChunkThreadedCache()
{
}

void OmThreadChunkThreadedCache::GetChunk(QExplicitlySharedDataPointer<OmThreadChunkLevel>& p_value, const OmMipChunkCoord& key, bool block)
{
	OmThreadedCache<OmMipChunkCoord, OmThreadChunkLevel>::Get(p_value, key, block);
}

OmThreadChunkLevel* OmThreadChunkThreadedCache::HandleCacheMiss(const OmMipChunkCoord &key)
{
	assert(mMipVolume->ContainsThreadChunkCoord(key));

	return new OmThreadChunkLevel(key, mMipVolume);	
}

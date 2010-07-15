#include "omMipVolume.h"
#include "omMipChunk.h"
#include "omThreadChunkLevel.h"
#include "omVolume.h"
#include "omThreadChunkThreadedCache.h"

OmThreadChunkThreadedCache::OmThreadChunkThreadedCache(OmMipVolume * volume) : OmThreadedCache<OmMipChunkCoord, OmThreadChunkLevel>(RAM_CACHE_GROUP)
{
        int chunkDim = volume->GetThreadChunkDimension();
        SetObjectSize(chunkDim*chunkDim*chunkDim*volume->GetBytesPerSample());
	mMipVolume = volume;

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

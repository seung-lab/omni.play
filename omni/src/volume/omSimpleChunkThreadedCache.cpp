#include "omMipVolume.h"
#include "omMipChunk.h"
#include "omSimpleChunk.h"
#include "omVolume.h"
#include "omSimpleChunkThreadedCache.h"

OmSimpleChunkThreadedCache::OmSimpleChunkThreadedCache(OmMipVolume * volume) : SimpleChunkThreadedCache(RAM_CACHE_GROUP)
{
	SetCacheName("SimpleChunkThreadedCache");
	mMipVolume = volume;

}

OmSimpleChunkThreadedCache::~OmSimpleChunkThreadedCache()
{

}

void
OmSimpleChunkThreadedCache::GetChunk(QExplicitlySharedDataPointer<OmSimpleChunk>& p_value, const OmMipChunkCoord& key, bool block)
{
	SimpleChunkThreadedCache::Get(p_value, key, block);
}


OmSimpleChunk* 
OmSimpleChunkThreadedCache::HandleCacheMiss(const OmMipChunkCoord &key)
{
	assert(mMipVolume->ContainsMipChunkCoord(key));

	return new OmSimpleChunk(key, mMipVolume);	
}



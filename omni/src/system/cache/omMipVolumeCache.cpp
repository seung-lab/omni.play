#include "volume/omMipChunk.h"
#include "system/cache/omMipVolumeCache.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omMipVolume.h"

#include <boost/make_shared.hpp>

static const int NUM_THREADS = 0; // no threads needed for mem-mapped volume

OmMipVolumeCache::OmMipVolumeCache(OmMipVolume* parent)
	: OmThreadedCache<OmMipChunkCoord,
					  OmMipChunkPtr>(RAM_CACHE_GROUP,
									 parent->GetName(),
									 NUM_THREADS)
	, mMipVolume(parent)
{
}

OmMipChunkPtr
OmMipVolumeCache::HandleCacheMiss(const OmMipChunkCoord &rMipCoord){
	assert(mMipVolume->ContainsMipChunkCoord(rMipCoord));
	return OmMipChunkPtr(boost::make_shared<OmMipChunk>(rMipCoord, mMipVolume));
}

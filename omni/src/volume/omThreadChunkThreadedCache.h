#ifndef OM_THREAD_CHUNK_THREADED_CACHE_H
#define OM_THREAD_CHUNK_THREADED_CACHE_H

#include "omMipChunkCoord.h"
#include "system/cache/omThreadedCache.h"
#include "omThreadChunkLevel.h"

/*
 *	A Cache that stores OmThreadChunkLevels
*/

class OmThreadChunkThreadedCache : public OmThreadedCache<OmMipChunkCoord, OmThreadChunkLevel> {

 public:
	OmThreadChunkThreadedCache(OmMipVolume* volume);
	~OmThreadChunkThreadedCache();
	void GetChunk(QExplicitlySharedDataPointer<OmThreadChunkLevel>& p_value, const OmMipChunkCoord& key, bool block=false);

 private:
	OmThreadChunkLevel* HandleCacheMiss(const OmMipChunkCoord &key);
	OmMipVolume* mMipVolume;
};

#endif

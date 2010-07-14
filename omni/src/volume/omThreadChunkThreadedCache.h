#ifndef OM_THREAD_CHUNK_THREADED_CACHE_H
#define OM_THREAD_CHUNK_THREADED_CACHE_H

#include "omMipChunkCoord.h"
#include "system/omThreadedCache.h"
#include "omThreadChunkLevel.h"

typedef OmThreadedCache< OmMipChunkCoord, OmThreadChunkLevel > ThreadChunkThreadedCache;

/*
 *	A Cache that stores OmThreadChunkLevels
*/

class OmThreadChunkThreadedCache : public ThreadChunkThreadedCache {

 public:
	OmThreadChunkThreadedCache(OmMipVolume* volume);
	~OmThreadChunkThreadedCache();
	void GetChunk(QExplicitlySharedDataPointer<OmThreadChunkLevel>& p_value, const OmMipChunkCoord& key, bool block=false);

 private:
	OmThreadChunkLevel* HandleCacheMiss(const OmMipChunkCoord &key);
	OmMipVolume* mMipVolume;
};

#endif

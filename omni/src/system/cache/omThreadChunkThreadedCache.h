#ifndef OM_THREAD_CHUNK_THREADED_CACHE_H
#define OM_THREAD_CHUNK_THREADED_CACHE_H

#include "system/cache/omThreadedCache.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omThreadChunkLevel.h"

/*
 *	A Cache that stores OmThreadChunkLevels
*/

class OmThreadChunkThreadedCache
	: public OmThreadedCache<OmMipChunkCoord, OmThreadChunkLevelPtr> {
public:
	OmThreadChunkThreadedCache(OmMipVolume* volume);
	~OmThreadChunkThreadedCache();

	void GetChunk(OmThreadChunkLevelPtr& p_value,
		      const OmMipChunkCoord& key,
		      bool block=false);

private:
	OmThreadChunkLevelPtr HandleCacheMiss(const OmMipChunkCoord &key);

	OmMipVolume *const mMipVolume;
};

#endif

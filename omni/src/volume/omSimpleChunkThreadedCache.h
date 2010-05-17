#ifndef OM_SIMPLE_CHUNK_THREADED_CACHE_TILE_H
#define OM_SIMPLE_CHUNK_THREADED_CACHE_TILE_H

#include "omMipChunkCoord.h"
#include "system/omThreadedCache.h"
#include "omSimpleChunk.h"

typedef OmThreadedCache< OmMipChunkCoord, OmSimpleChunk > SimpleChunkThreadedCache;


 /**
 *	A Cache that stores OmSimpleChunks . . . 
 *      which are chunks of specimen data that 
 *      are not contained in a VTK wrapper.
 *
 *	Clay Didier - csdidier@mit.edu
 */

class OmSimpleChunkThreadedCache : public SimpleChunkThreadedCache {

 public:
	OmSimpleChunkThreadedCache(OmMipVolume* volume);
	~OmSimpleChunkThreadedCache();
	void GetChunk(QExplicitlySharedDataPointer<OmSimpleChunk>& p_value, const OmMipChunkCoord& key, bool block=false);

	//void Flush();

 private:
	OmSimpleChunk* HandleCacheMiss(const OmMipChunkCoord &key);
	OmMipVolume* mMipVolume;
	//void HandleFetchUpdate();
	//bool InitializeFetchThread();
	//bool KillFetchThread();
};

#endif

#ifndef OM_SEGMENTATION_CACHE_H
#define OM_SEGMENTATION_CACHE_H

#include "system/omThreadedCache.h"
#include "omMipChunkCoord.h"
#include "volume/omMipChunk.h"

class OmMipVolumeCache;

class OmMipVolumeCache : public OmThreadedCache<OmMipChunkCoord, OmMipChunk> {
public:
	OmMipVolumeCache(OmMipVolume * parent);
	
	OmMipChunk* HandleCacheMiss(const OmMipChunkCoord &key);

private:
	OmMipVolume *const mMipVolume;
};

#endif

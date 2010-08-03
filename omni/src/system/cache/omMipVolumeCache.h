#ifndef OM_SEGMENTATION_CACHE_H
#define OM_SEGMENTATION_CACHE_H

#include "system/cache/omThreadedCache.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omMipChunk.h"

typedef boost::shared_ptr<OmMipChunk> OmMipChunkPtr;

class OmMipVolumeCache;

class OmMipVolumeCache : public OmThreadedCache<OmMipChunkCoord, OmMipChunkPtr> {
public:
	OmMipVolumeCache(OmMipVolume * parent);
	
	OmMipChunkPtr HandleCacheMiss(const OmMipChunkCoord &key);

private:
	OmMipVolume *const mMipVolume;
};

#endif

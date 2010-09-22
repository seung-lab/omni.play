#ifndef OM_MIP_VOLUME_CACHE_H
#define OM_MIP_VOLUME_CACHE_H

#include "system/cache/omThreadedCache.h"
#include "volume/omVolumeTypes.hpp"

class OmMipVolume;
class OmMipChunkCoord;

class OmMipVolumeCache : public OmThreadedCache<OmMipChunkCoord, OmMipChunkPtr> {
public:
	OmMipVolumeCache(OmMipVolume*);

	OmMipChunkPtr HandleCacheMiss(const OmMipChunkCoord &key);

private:
	OmMipVolume *const mMipVolume;
};

#endif

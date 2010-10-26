#ifndef OM_MIP_VOLUME_CACHE_H
#define OM_MIP_VOLUME_CACHE_H

#include "zi/omMutex.h"
#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"
#include "volume/omMipChunkCoord.h"

#include <boost/make_shared.hpp>

class OmMipVolumeCache{
public:
	OmMipVolumeCache(OmMipVolume* vol)
		: vol_(vol)
	{}

	void Get(OmMipChunkPtr& ptr, const OmMipChunkCoord& coord)
	{
		zi::guard g(mutex_);
		if(map_.count(coord)){
			ptr = map_[coord];
		}else{
			assert(vol_->ContainsMipChunkCoord(coord));
			ptr = map_[coord] =
				OmMipChunkPtr(boost::make_shared<OmMipChunk>(coord, vol_));
		}
	}

private:
	OmMipVolume *const vol_;;
	zi::mutex mutex_;
	std::map<OmMipChunkCoord, OmMipChunkPtr> map_;
};

#endif

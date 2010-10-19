#ifndef OM_RAW_CHUNK_CACHE_HPP
#define OM_RAW_CHUNK_CACHE_HPP

#include "system/cache/omThreadedCache.h"
#include "volume/omMipVolume.h"
#include "volume/omRawChunk.hpp"
#include "volume/omMipChunkCoord.h"

template <typename T>
class OmRawChunkCache
	: public OmThreadedCache<OmMipChunkCoord,
							 boost::shared_ptr<OmRawChunk<T> > > {
public:
	OmRawChunkCache(OmMipVolume* vol)
		: OmThreadedCache<OmMipChunkCoord,
						  boost::shared_ptr<OmRawChunk<T> > >
		  (RAM_CACHE_GROUP, "raw chunks", 0)
		, vol_(vol)
	{}

	boost::shared_ptr<OmRawChunk<T> >
	HandleCacheMiss(const OmMipChunkCoord& coord)
	{
		return boost::make_shared<OmRawChunk<T> >(vol_, coord);
	}

private:
	OmMipVolume *const vol_;
};

#endif

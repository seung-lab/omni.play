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
		  (RAM_CACHE_GROUP,
		   "raw chunks",
		   0,
		   om::DONT_THROTTLE)
		, vol_(vol)
	{}

	/* TODO: test me
	~OmRawChunkCache()
	{
		UpdateSize(chunk->NumBytes());
	}
	*/

	boost::shared_ptr<OmRawChunk<T> >
	HandleCacheMiss(const OmMipChunkCoord& coord)
	{
		OmRawChunk<T>* chunk = new OmRawChunk<T>(vol_, coord);
		UpdateSize(chunk->NumBytes());
		return boost::shared_ptr<OmRawChunk<T> >(chunk);
	}

private:
	OmMipVolume *const vol_;
};

#endif

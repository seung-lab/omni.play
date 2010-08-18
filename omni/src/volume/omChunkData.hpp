#ifndef OM_CHUNK_DATA_HPP
#define OM_CHUNK_DATA_HPP

#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"

#include <zi/mutex>

class OmChunkData {
public:
	OmChunkData(OmMipVolume* vol, const OmMipChunkCoord &coord)
		: rawData(vol->volData->getChunkPtrRaw(coord))
		, vol_(vol)
		, coord_(coord)
	{}

	OmRawDataPtrs rawData;

private:
	OmMipVolume *const vol_;
	const OmMipChunkCoord coord_;

};

#endif

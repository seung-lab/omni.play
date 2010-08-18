#ifndef OM_CHUNK_DATA_HPP
#define OM_CHUNK_DATA_HPP

#include "volume/omMipVolume.h"
#include <zi/mutex>

class OmChunkData {
public:
	OmChunkData(OmMipVolume* vol, const OmMipChunkCoord &coord)
		: vol_(vol), coord_(coord)
	{
		rawData = vol_->volData->getChunkPtrRaw(coord);
	}

	OmRawDataPtrs rawData;

private:
	OmMipVolume *const vol_;
	const OmMipChunkCoord coord_;

};

#endif

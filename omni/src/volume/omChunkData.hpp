#ifndef OM_CHUNK_DATA_HPP
#define OM_CHUNK_DATA_HPP

#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"

#include <zi/mutex>

class OmChunkData {
public:
	OmChunkData(OmMipVolume* vol, const OmMipChunkCoord &coord);

	OmRawDataPtrs rawData;

	void* ExtractDataSlice(const ViewType plane, const int offset);
	OmSegSizeMapPtr RefreshDirectDataValues(OmMipChunk* chunk,
							     const bool computeSizes);
private:
	OmMipVolume *const vol_;
	const OmMipChunkCoord coord_;
};

#endif

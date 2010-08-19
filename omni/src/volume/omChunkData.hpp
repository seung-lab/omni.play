#ifndef OM_CHUNK_DATA_HPP
#define OM_CHUNK_DATA_HPP

#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"

#include <zi/mutex>

class OmChunkData {
public:
	OmChunkData(OmMipVolume*, OmMipChunk*, const OmMipChunkCoord& coord);

	OmRawDataPtrs rawData;

	void* ExtractDataSlice(const ViewType plane, const int offset);
	OmSegSizeMapPtr RefreshDirectDataValues(const bool computeSizes);
	void copyInTile(const int sliceOffset, uchar* bits);
	void copyChunkFromMemMapToHDF5();
	void copyDataFromHDF5toMemMap();

private:
	OmMipVolume *const vol_;
	OmMipChunk *const chunk_;
	const OmMipChunkCoord coord_;
};

#endif

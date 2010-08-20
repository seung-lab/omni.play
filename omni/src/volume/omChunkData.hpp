#ifndef OM_CHUNK_DATA_HPP
#define OM_CHUNK_DATA_HPP

#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"

#include <zi/mutex>

class OmChunkData {
public:
	OmChunkData(OmMipVolume*, OmMipChunk*, const OmMipChunkCoord& coord);

	OmRawDataPtrs& getRawData();

	OmSegSizeMapPtr RefreshDirectDataValues(const bool computeSizes);

	void* ExtractDataSlice(const ViewType plane, const int offset);
	OmImage<uint32_t, 3> getOmImage32Chunk();

	void copyInTile(const int sliceOffset, uchar* bits);
	void copyChunkFromMemMapToHDF5();
	void copyDataFromHDF5toMemMap();
	void copyDataFromHDF5toMemMap(OmDataWrapperPtr hdf5);

private:
	OmMipVolume *const vol_;
	OmMipChunk *const chunk_;
	const OmMipChunkCoord coord_;
	bool loadedData_;

	OmRawDataPtrs rawData_;
};

#endif

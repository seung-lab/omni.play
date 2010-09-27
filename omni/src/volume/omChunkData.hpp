#ifndef OM_CHUNK_DATA_HPP
#define OM_CHUNK_DATA_HPP

#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"
#include "utility/image/omImage.hpp"

#include "zi/omMutex.h"

class OmSegmentCache;

class OmChunkData {
public:
	OmChunkData(OmMipVolume*, OmMipChunk*, const OmMipChunkCoord& coord);

	OmRawDataPtrs& getRawData();

	void RefreshDirectDataValues(const bool,
								 boost::shared_ptr<OmSegmentCache>);

	boost::shared_ptr<uint8_t>  ExtractDataSlice8bit(const ViewType, const int);
	boost::shared_ptr<uint32_t> ExtractDataSlice32bit(const ViewType, const int);
	OmImage<uint32_t, 3> GetCopyOfChunkDataAsOmImage32();

	void copyInTile(const int sliceOffset, uchar* bits);
	void copyDataFromHDF5toMemMap();
	void copyDataFromHDF5toMemMap(OmDataWrapperPtr hdf5);

	uint32_t SetVoxelValue(const DataCoord & voxel, uint32_t val);
	uint32_t GetVoxelValue(const DataCoord & voxel);

	bool compare(boost::shared_ptr<OmChunkData> other);

private:
	OmMipVolume *const vol_;
	OmMipChunk *const chunk_;
	const OmMipChunkCoord coord_;
	bool loadedData_;

	OmRawDataPtrs rawData_;
};

#endif

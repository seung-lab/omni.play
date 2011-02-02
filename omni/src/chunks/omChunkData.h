#ifndef OM_CHUNK_DATA_HPP
#define OM_CHUNK_DATA_HPP

#include "chunks/omChunkCoord.h"
#include "chunks/omChunkTypes.hpp"
#include "datalayer/omDataWrapper.h"
#include "utility/image/omImage.hpp"
#include "volume/omVolumeTypes.hpp"

class OmMipVolume;

class OmChunkData {
public:
	OmChunkData(OmMipVolume*, OmChunk*);

	OmImage<uint8_t, 2> ExtractDataSlice8bit(const ViewType, const int);

	void copyInTile(const int sliceOffset, uchar* bits);
	void CopyInChunkData(OmDataWrapperPtr hdf5);
	OmDataWrapperPtr CopyOutChunkData();

	double GetMinValue();
	double GetMaxValue();

	bool Compare(OmChunkData* other);

private:
	OmMipVolume *const vol_;
	OmChunk *const chunk_;

	OmRawDataPtrs& getRawData();
};

#endif

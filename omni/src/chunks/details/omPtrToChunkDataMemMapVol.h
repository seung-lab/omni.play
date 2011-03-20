#ifndef OM_PTR_TO_CHUNK_DATA_MEM_MAP_VOL_HPP
#define OM_PTR_TO_CHUNK_DATA_MEM_MAP_VOL_HPP

#include "chunks/details/omPtrToChunkDataBase.hpp"
#include "zi/omMutex.h"

class OmPtrToChunkDataMemMapVol : public OmPtrToChunkDataBase {
private:
	OmMipVolume *const vol_;
	const OmChunkCoord coord_;

	bool loadedData_;
	OmRawDataPtrs rawData_;

	zi::rwmutex lock_;

public:
	OmPtrToChunkDataMemMapVol(OmMipVolume* vol, const OmChunkCoord& coord)
		: vol_(vol)
		, coord_(coord)
		, loadedData_(false)
	{}

	OmRawDataPtrs& GetRawData();
};

#endif

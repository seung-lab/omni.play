#ifndef OM_PTR_TO_CHUNK_DATA_BASE_HPP
#define OM_PTR_TO_CHUNK_DATA_BASE_HPP

#include "volume/omVolumeTypes.hpp"

class OmPtrToChunkDataBase {
public:
	OmPtrToChunkDataBase()
	{}

	virtual ~OmPtrToChunkDataBase()
	{}

	virtual OmRawDataPtrs& GetRawData() = 0;
};

#endif

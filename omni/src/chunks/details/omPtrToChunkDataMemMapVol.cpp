#include "volume/io/omVolumeData.h"
#include "chunks/details/omPtrToChunkDataMemMapVol.h"

OmRawDataPtrs& OmPtrToChunkDataMemMapVol::GetRawData()
{
	zi::rwmutex::write_guard g(lock_);

	if(!loadedData_){
		rawData_ = vol_->VolData()->getChunkPtrRaw(coord_);
		loadedData_ = true;
	}
	return rawData_;
}

#pragma once

#include "volume/io/omVolumeData.h"
#include "volume/omMipVolume.h"
#include "chunks/details/omPtrToChunkDataBase.hpp"
#include "zi/omMutex.h"

namespace om {
namespace chunk {

template <typename DATA>
class ptrToChunkDataMemMapVol : public ptrToChunkDataBase {
private:
    DATA *const rawData_;

public:
    ptrToChunkDataMemMapVol(OmMipVolume* vol, const OmChunkCoord& coord)
        : rawData_(boost::get<DATA*>(vol->VolData()->getChunkPtrRaw(coord)))
    {}

    inline DATA* GetRawData(DATA*){
        return rawData_;
    }
};

} // namespace chunk
} // namespace om

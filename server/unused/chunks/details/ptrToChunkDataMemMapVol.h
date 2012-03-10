#pragma once

#include "volume/io/volumeData.h"
#include "volume/volume.h"
#include "chunks/details/ptrToChunkDataBase.hpp"
#include "zi/mutex.h"

namespace om {
namespace chunks {

template <typename DATA>
class ptrToChunkDataMemMapVol : public ptrToChunkDataBase {
private:
    DATA *const rawData_;

public:
    ptrToChunkDataMemMapVol(volume::volume* vol, const coords::chunkCoord& coord)
        : rawData_(boost::get<DATA*>(vol->VolData()->getChunkPtrRaw(coord)))
    {}

    inline DATA* GetRawData(DATA*){
        return rawData_;
    }
};

} // namespace chunk
} // namespace om

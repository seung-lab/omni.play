#include "common/common.h"
#include "chunks/chunkData.hpp"
#include "volume/volume.h"
#include "volume/io/volumeData.h"

namespace om {
namespace chunks {

template <typename T>
chunk::chunk(volume::volume* vol, const coords::chunkCoord& coord)
    : coord_(coord)
    , data_(vol->VolData()->getChunkPtrRaw<T>(coord))
    , vol_(vol)
    , mipping_(vol, coord)
{}

}
}

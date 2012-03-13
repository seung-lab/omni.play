#include "common/common.h"
#include "chunks/chunk.h"
#include "chunks/chunkData.hpp"

namespace om {
namespace chunks {

chunk::chunk(volume::volume* vol, const coords::chunkCoord& coord)
    : coord_(coord)
    , chunkData_(dataFactory::Produce(vol, coord))
    , vol_(vol)
    , mipping_(new mipping(vol, coord))
{}

coords::dataCoord chunk::GetDimensions() const {
    return mipping_->GetExtent().getUnitDimensions();
}

}
}

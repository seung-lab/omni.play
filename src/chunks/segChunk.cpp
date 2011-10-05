#include "chunks/segChunk.h"
#include "volume/segmentation.h"
#include "chunks/segChunkData.hpp"

segChunk::segChunk(segmentation* vol, const om::chunkCoord& coord)
    : chunk(vol, coord)
    , vol_(vol)
    , segChunkData_(om::segchunk::dataFactory::Produce(vol, this, coord))
{}

segChunk::~segChunk()
{}

void segChunk::SetVoxelValue(const om::dataCoord& coord,
                               const uint32_t val)
{
    assert(ContainsVoxel(coord));

    const uint32_t oldVal = segChunkData_->SetVoxelValue(coord, val);

    {
        zi::guard g(modifiedSegIDsLock_);
        modifiedSegIDs_.insert(oldVal);
        modifiedSegIDs_.insert(val);
    }
}

uint32_t segChunk::GetVoxelValue(const om::dataCoord& coord)
{
    assert(ContainsVoxel(coord));

    return segChunkData_->GetVoxelValue(coord);
}

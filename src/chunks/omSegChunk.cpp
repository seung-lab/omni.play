#include "chunks/omSegChunk.h"
#include "volume/segmentation.h"
#include "chunks/omSegChunkData.hpp"

OmSegChunk::OmSegChunk(segmentation* vol, const om::chunkCoord& coord)
    : OmChunk(vol, coord)
    , vol_(vol)
    , segChunkData_(om::segchunk::dataFactory::Produce(vol, this, coord))
{}

OmSegChunk::~OmSegChunk()
{}

void OmSegChunk::SetVoxelValue(const om::dataCoord& coord,
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

uint32_t OmSegChunk::GetVoxelValue(const om::dataCoord& coord)
{
    assert(ContainsVoxel(coord));

    return segChunkData_->GetVoxelValue(coord);
}

#include "chunks/omSegChunk.h"
#include "volume/omSegmentation.h"

OmSegChunk::OmSegChunk(OmSegmentation* vol, const OmChunkCoord& coord)
    : OmChunk(vol, coord)
    , vol_(vol)
    , segChunkData_(new OmSegChunkData(vol, this, coord))
{}

OmSegChunk::~OmSegChunk()
{}

void OmSegChunk::SetVoxelValue(const DataCoord& globalCoord,
                               const uint32_t val)
{
    assert(ContainsVoxel(globalCoord));

    const DataCoord voxel = globalCoord - mipping_.GetExtent().getMin();
    const uint32_t oldVal = segChunkData_->SetVoxelValue(voxel, val);

    {
        zi::guard g(modifiedSegIDsLock_);
        modifiedSegIDs_.insert(oldVal);
        modifiedSegIDs_.insert(val);
    }
}

uint32_t OmSegChunk::GetVoxelValue(const DataCoord& globalCoord)
{
    assert(ContainsVoxel(globalCoord));

    const DataCoord voxel = globalCoord - mipping_.GetExtent().getMin();

    return segChunkData_->GetVoxelValue(voxel);
}

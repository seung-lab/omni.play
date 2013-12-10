#include "chunks/omSegChunk.h"
#include "volume/omSegmentation.h"
#include "chunks/omSegChunkData.hpp"

OmSegChunk::OmSegChunk(OmSegmentation* vol, const om::coords::Chunk& coord)
    : OmChunk(vol, coord),
      vol_(vol),
      segChunkData_(om::segchunk::dataFactory::Produce(vol, this, coord)) {}

OmSegChunk::~OmSegChunk() {}

void OmSegChunk::SetVoxelValue(const om::coords::Data& coord,
                               const uint32_t val) {
  assert(ContainsVoxel(coord));

  const uint32_t oldVal = segChunkData_->SetVoxelValue(coord, val);

  {
    zi::guard g(modifiedSegIDsLock_);
    modifiedSegIDs_.insert(oldVal);
    modifiedSegIDs_.insert(val);
  }
}

uint32_t OmSegChunk::GetVoxelValue(const om::coords::Data& coord) {
  assert(ContainsVoxel(coord));

  return segChunkData_->GetVoxelValue(coord);
}

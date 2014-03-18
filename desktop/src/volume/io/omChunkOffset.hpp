#pragma once
#include "precomp.h"

#include "volume/omMipVolume.h"

class OmChunkOffset {
 public:
  static uint64_t ComputeChunkPtrOffsetBytes(OmMipVolume* vol,
                                             const om::coords::Chunk& coord) {
    const int level = coord.mipLevel();
    const Vector3<int64_t> volDims =
        vol->Coords().DimsRoundedToNearestChunk(level);
    const Vector3<int64_t> chunkDims = vol->Coords().ChunkDimensions();
    const int64_t bps = vol->GetBytesPerVoxel();

    const int64_t slabSize = volDims.x * volDims.y * chunkDims.z * bps;
    const int64_t rowSize = volDims.x * chunkDims.y * chunkDims.z * bps;
    const int64_t chunkSize = chunkDims.x * chunkDims.y * chunkDims.z * bps;

    const Vector3<int64_t> chunkPos = coord;  // bottom left corner
    const int64_t offset =
        slabSize * chunkPos.z + rowSize * chunkPos.y + chunkSize * chunkPos.x;

    return offset;
  }
};

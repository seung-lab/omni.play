#include "common/common.h"
#include "chunks/omChunkData.hpp"

OmChunk::OmChunk(OmMipVolume& vol, const om::coords::Chunk& coord)
    : coord_(coord),
      chunkData_(om::chunk::dataFactory::Produce(&vol, coord)),
      vol_(vol),
      mipping_(&vol, coord) {}

OmChunk::~OmChunk() {}

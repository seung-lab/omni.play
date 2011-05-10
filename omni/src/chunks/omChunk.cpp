#include "chunks/omChunk.h"
#include "chunks/omChunkData.hpp"

OmChunk::OmChunk(OmChannel* vol, const OmChunkCoord& coord)
    : coord_(coord)
    , chunkData_(om::chunk::dataFactory::Produce(vol, coord))
{
    mipping_.Init(vol, coord);
}

OmChunk::OmChunk(OmChannelImpl* vol, const OmChunkCoord& coord)
    : coord_(coord)
    , chunkData_(om::chunk::dataFactory::Produce(vol, coord))
{
    mipping_.Init(vol, coord);
}

OmChunk::OmChunk(OmSegmentation* vol, const OmChunkCoord& coord)
    : coord_(coord)
    , chunkData_(om::chunk::dataFactory::Produce(vol, coord))
{
    mipping_.Init(vol, coord);
}

OmChunk::~OmChunk()
{}

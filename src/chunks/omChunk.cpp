#include "common/common.h"
#include "chunks/omChunkData.hpp"

OmChunk::OmChunk(channel* vol, const om::chunkCoord& coord)
    : coord_(coord)
    , chunkData_(om::chunk::dataFactory::Produce(vol, coord))
    , vol_(vol)
    , mipping_(vol, coord)
{}

OmChunk::OmChunk(channelImpl* vol, const om::chunkCoord& coord)
    : coord_(coord)
    , chunkData_(om::chunk::dataFactory::Produce(vol, coord))
    , vol_(vol)
    , mipping_(vol, coord)
{}

OmChunk::OmChunk(segmentation* vol, const om::chunkCoord& coord)
    : coord_(coord)
    , chunkData_(om::chunk::dataFactory::Produce(vol, coord))
    , vol_(vol)
    , mipping_(vol, coord)
{}

OmChunk::~OmChunk()
{}

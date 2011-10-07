#include "common/common.h"
#include "chunks/chunkData.hpp"

chunk::chunk(channel* vol, const coords::chunkCoord& coord)
    : coord_(coord)
    , chunkData_(om::chunk::dataFactory::Produce(vol, coord))
    , vol_(vol)
    , mipping_(vol, coord)
{}

chunk::chunk(channelImpl* vol, const coords::chunkCoord& coord)
    : coord_(coord)
    , chunkData_(om::chunk::dataFactory::Produce(vol, coord))
    , vol_(vol)
    , mipping_(vol, coord)
{}

chunk::chunk(segmentation* vol, const coords::chunkCoord& coord)
    : coord_(coord)
    , chunkData_(om::chunk::dataFactory::Produce(vol, coord))
    , vol_(vol)
    , mipping_(vol, coord)
{}

chunk::~chunk()
{}

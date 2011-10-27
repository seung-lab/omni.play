#include "pipeline/getTileData.h"
#include "volume/channel.h"
#include "tiles/tile.h"
#include "chunks/chunk.h"
#include "chunks/chunkDataInterface.hpp"

namespace om {
namespace pipeline {

char* getTileData::operator()()
{
    boost::shared_ptr<chunks::chunk> chunk = chan_->GetChunk(coord_);
    std::cout << chunk << chunk->Data() << std::endl;
    data_ = reinterpret_cast<char*>(chunk->Data()->ExtractDataSlice8bit(view_, depth_).get());
    return data_;
}

void getTileData::cleanup()
{
    delete data_;
}

}
}

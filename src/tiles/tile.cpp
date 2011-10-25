#include "chunks/chunkData.hpp"
#include "tiles/tile.h"
#include "volume/volume.h"
#include "chunks/chunk.h"
#include "chunks/segChunk.h"
#include "chunks/segChunkDataInterface.hpp"

namespace om {
namespace tiles {

void tile::loadData()
{
    if(vol_->getVolumeType() == common::CHANNEL) {
        volume::channel* chan = reinterpret_cast<volume::channel*>(vol_);
        boost::shared_ptr<chunks::chunk> chunk = chan->GetChunk(coord_);

        data_ = chunk->Data()->ExtractDataSlice8bit(view_, depth_);

    } else {
        volume::segmentation* seg = reinterpret_cast<volume::segmentation*>(vol_);
        boost::shared_ptr<chunks::segChunk> chunk = seg->GetChunk(coord_);

        data_ = chunk->SegData()->ExtractDataSlice32bit(view_, depth_);
    }
}

}
}

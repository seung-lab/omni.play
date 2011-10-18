#include "chunks/chunkData.hpp"
#include "tiles/tile.h"
#include "utility/dataWrappers.h"
#include "volume/volume.h"

void tile::loadData()
{
    if(getVolType() == CHANNEL) {
        channel* chan = reinterpret_cast<channel*>(vol_);
        chunk* chunk = chan->GetChunk(mipChunkCoord_);

        data_ = chunk->Data()->ExtractDataSlice8bit(key_.getViewType(),
                                                    getChunkSliceNum());

    } else {
        segmentation* seg = reinterpret_cast<segmentation*>(vol_);
        segChunk* chunk = seg->GetChunk(mipChunkCoord_);

        data_ = chunk->SegData()->ExtractDataSlice32bit(viewType_,
                                                        getChunkSliceNum());
    }
}

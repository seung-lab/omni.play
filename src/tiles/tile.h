#pragma once

#include "common/std.h"
#include "tiles/tileCoord.h"

namespace om {
namespace volume {
class volume;
}

namespace om {
namespace tiles {

template <typename T>
class tile {
public:
    tile(const om::volume::volume* vol,
         coords::chunkCoord coord,
         common::viewType view,
         int depth)
        : vol_(vol)
        , coord_(coord)
        , view_(view)
        , depth_(depth)
    {}

    void LoadData()
    {
        if(vol_->
        channel* chan = reinterpret_cast<channel*>(getVol());
        chunk* chunk = chan->GetChunk(mipChunkCoord_);

        OmPooledTile<uint8_t>* tileData =
            chunk->Data()->ExtractDataSlice8bit(key_.getViewType(),
                                                getChunkSliceNum());

        channelTileFilter::Filter(tileData);

        texture_.reset(new OmTextureID(tileLength_, tileData));

        segmentation* seg = reinterpret_cast<segmentation*>(getVol());
        segChunk* chunk = seg->GetChunk(mipChunkCoord_);

        PooledTile32Ptr imageData =
            chunk->SegData()->ExtractDataSlice32bit(key_.getViewType(),
                                                    getChunkSliceNum());

        OmPooledTile<OmColorARGB>* colorMappedData =
            key_.getViewGroupState()->ColorTile(imageData->GetData(),
                                                tileLength_,
                                                key_);

        texture_.reset(new OmTextureID(tileLength_, colorMappedData));
    }

    const T* data() const {
        return data_;
    }

private:
    const om::volume::volume* vol_;
    const coords::chunkCoord coord_;
    const common::viewType view_;
    const int depth;

    T* data_;
};

} // namespace tiles
} // namespace om

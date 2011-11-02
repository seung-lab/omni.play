#pragma once

#include "common/std.h"
#include "chunks/chunk.h"
#include "chunks/segChunk.h"
#include "chunks/chunkDataInterface.hpp"
#include "chunks/segChunkDataInterface.hpp"
#include "tiles/tileData.hpp"
#include "volume/channel.h"
#include "volume/segmentation.h"

namespace om {
namespace volume { class volume; }

namespace tiles {

class tile {
public:
    tile(volume::volume* vol,
         coords::chunkCoord coord,
         common::viewType view,
         int depth)
        : vol_(vol)
        , coord_(coord)
        , view_(view)
        , depth_(depth)
    {}

    void loadData()
    {
        if(vol_->getVolumeType() == common::CHANNEL) {
            volume::channel* chan = reinterpret_cast<volume::channel*>(vol_);
            boost::shared_ptr<chunks::chunk> chunk = chan->GetChunk(coord_);

            data_.reset(new tileData<uint8_t>(chunk->Data()->ExtractDataSlice8bit(view_, depth_)));

        } else {
            volume::segmentation* seg = reinterpret_cast<volume::segmentation*>(vol_);
            boost::shared_ptr<chunks::segChunk> chunk = seg->GetChunk(coord_);

            data_.reset(new tileData<uint32_t>(chunk->SegData()->ExtractDataSlice32bit(view_, depth_)));
        }
    }

    void* data() {
        return data_->data();
    }

    const void* data() const {
        return data_->data();
    }

    template<typename T>
    T* data() {
        return reinterpret_cast<T*>(data_->data());
    }

    template<typename T>
    const T* data() const {
        return reinterpret_cast<const T*>(data_->data());
    }

    inline const volume::volume* volume() const {
        return vol_;
    }

    inline const coords::chunkCoord coord() const {
        return coord_;
    }

    inline common::viewType view() const {
        return view_;
    }

    inline int depth() const {
        return depth_;
    }
private:
    volume::volume* vol_;
    const coords::chunkCoord coord_;
    const common::viewType view_;
    const int depth_;

    boost::scoped_ptr<tileDataInterface> data_;
};

} // namespace tiles
} // namespace om

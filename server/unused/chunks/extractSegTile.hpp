#pragma once

#include "tiles/tileFilters.hpp"
#include "chunks/rawChunkSlicer.hpp"
#include "chunks/segChunk.h"
#include "volume/segmentation.h"

namespace om {
namespace chunks {

class extractSegTile{
public:
    extractSegTile(volume::segmentation* vol, const coords::chunkCoord& coord,
                   const common::viewType plane, int depth)
        : vol_(vol)
        , coord_(coord)
        , plane_(plane)
        , depth_(depth)
    {}

    template <typename T>
    inline boost::shared_ptr<uint32_t> Extract(T* d) const {
        return getTile(d);
    }

    boost::shared_ptr<uint32_t> Extract(float*) const {
        throw common::ioException("segmentation data shouldn't be float");
    }

private:
    boost::shared_ptr<uint32_t> getTile(uint32_t* d) const
    {
        rawChunkSlicer<uint32_t> slicer(128, d);

        project::project::FileReadSemaphore().acquire(1);
        boost::shared_ptr<uint32_t> tile = slicer.GetCopyOfTile(plane_, depth_);
        project::project::FileReadSemaphore().release(1);

        return tile;
    }

    template <typename T>
    boost::shared_ptr<uint32_t> getTile(T* d) const
    {
        rawChunkSlicer<T> slicer(128, d);

        project::project::FileReadSemaphore().acquire(1);
        boost::shared_ptr<T> rawTile = slicer.GetCopyOfTile(plane_, depth_);
        project::project::FileReadSemaphore().release(1);

        tiles::filters<T> filter(128);

        return filter.template recast<uint32_t>(rawTile);
    }

    volume::segmentation *const vol_;
    const coords::chunkCoord coord_;
    const common::viewType plane_;
    const int depth_;
};

} // namespace segchunk
} // namespace om

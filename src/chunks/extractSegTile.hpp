#pragma once

#include "tiles/tileFilters.hpp"
#include "chunks/rawChunkSlicer.hpp"
#include "chunks/segChunk.h"
#include "tiles/cache/raw/omRawSegTileCache.hpp"
#include "volume/segmentation.h"

namespace om {
namespace segchunk {

class OmExtractSegTile{
public:
    OmExtractSegTile(segmentation* vol, const coords::chunkCoord& coord,
                     const common::viewType plane, int depth)
        : vol_(vol)
        , coord_(coord)
        , plane_(plane)
        , depth_(depth)
    {}

    template <typename T>
    inline PooledTile32Ptr Extract(T* d) const {
        return getCachedTile(d);
    }

    PooledTile32Ptr Extract(float*) const {
        throw common::ioException("segmentation data shouldn't be float");
    }

private:
    template <typename T>
    PooledTile32Ptr getCachedTile(T* d) const
    {
        PooledTile32Ptr dataPtr = vol_->SliceCache()->Get(coord_, depth_, plane_);

        if(!dataPtr)
        {
            dataPtr = getTile(d);
            vol_->SliceCache()->Set(coord_, depth_, plane_, dataPtr);
        }

        return dataPtr;
    }

    PooledTile32Ptr getTile(uint32_t* d) const
    {
        rawChunkSlicer<uint32_t> slicer(128, d);

        project::Globals().FileReadSemaphore().acquire(1);
        OmPooledTile<uint32_t>* tile = slicer.GetCopyAsPooledTile(plane_, depth_);
        project::Globals().FileReadSemaphore().release(1);

        return PooledTile32Ptr(tile);
    }

    template <typename T>
    PooledTile32Ptr getTile(T* d) const
    {
        rawChunkSlicer<T> slicer(128, d);

        project::Globals().FileReadSemaphore().acquire(1);
        boost::scoped_ptr<OmPooledTile<T> > rawTile(slicer.GetCopyAsPooledTile(plane_, depth_));
        project::Globals().FileReadSemaphore().release(1);

        tileFilters<T> filter(128);

        OmPooledTile<uint32_t>* tile = filter.recastToUint32(rawTile.get());

        return PooledTile32Ptr(tile);
    }

    segmentation *const vol_;
    const coords::chunkCoord coord_;
    const common::viewType plane_;
    const int depth_;
};

} // namespace segchunk
} // namespace om

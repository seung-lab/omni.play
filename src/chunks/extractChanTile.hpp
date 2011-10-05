#pragma once

#include "tiles/omTileFilters.hpp"
#include "chunks/rawChunkSlicer.hpp"
#include "volume/mipVolume.h"

namespace om {
namespace chunk {

class extractChanTile{
private:
    mipVolume *const vol_;
    const om::chunkCoord coord_;
    const ViewType plane_;
    const int depth_;

public:
    extractChanTile(mipVolume* vol, const om::chunkCoord& coord, const ViewType plane, int depth)
        : vol_(vol)
        , coord_(coord)
        , plane_(plane)
        , depth_(depth)
    {}

    template <typename T>
    OmPooledTile<uint8_t>* Extract(T* d){
        return extractDataSlice8bit(d);
    }

private:
    template <typename T>
    OmPooledTile<uint8_t>* extractDataSlice8bit(T* d)
    {
        boost::scoped_ptr<OmPooledTile<T> > rawTile(getRawSlice(d));
        OmTileFilters<T> filter(128);
        return filter.recastToUint8(rawTile.get());
    }

    OmPooledTile<uint8_t>* extractDataSlice8bit(uint8_t* d){
        return getRawSlice(d);
    }

    OmPooledTile<uint8_t>* extractDataSlice8bit(float* d)
    {
        boost::scoped_ptr<OmPooledTile<float> > rawTile(getRawSlice(d));

        OmTileFilters<float> filter(128);

        float mn = 0.0;
        float mx = 1.0;

        // TODO: use actual range in channel data
        // mpMipVolume->GetBounds(mx, mn);

        return filter.rescaleAndCast<uint8_t>(rawTile.get(), mn, mx, 255.0);
    }

    template <typename T>
    inline OmPooledTile<T>* getRawSlice(T* d) const
    {
        rawChunkSlicer<T> slicer(128, d);

        project::Globals().FileReadSemaphore().acquire(1);
        OmPooledTile<T>* tile = slicer.GetCopyAsPooledTile(plane_, depth_);
        project::Globals().FileReadSemaphore().release(1);

        return tile;
    }
};

} // namespace chunk
} // namespace om

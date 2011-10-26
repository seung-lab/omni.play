#pragma once

#include "chunks/rawChunkSlicer.hpp"
#include "volume/volume.h"
#include "tiles/tileFilters.hpp"

namespace om {
namespace chunks {

class extractChanTile{
private:
    volume::volume *const vol_;
    const int chunkDim_;
    const int chunkSize_;
    const coords::chunkCoord coord_;
    const common::viewType plane_;
    const int depth_;

public:
    extractChanTile(volume::volume* vol, const coords::chunkCoord& coord,
                    const common::viewType plane, int depth)
        : vol_(vol)
        , chunkDim_(vol->CoordinateSystem().GetChunkDimension())
        , chunkSize_(chunkDim_ * chunkDim_)
        , coord_(coord)
        , plane_(plane)
        , depth_(depth)
    {}

    template <typename T>
    char* Extract(T* d){
        return extractDataSlice(d);
    }

private:
    template <typename T>
    char* extractDataSlice(T* d)
    {
        T* rawTile = getRawSlice(d);
        return reinterpret_cast<char*>(rawTile);
    }

    char* extractDataSlice(char* d){
        return getRawSlice(d);
    }

    char* extractDataSlice(float* d)
    {
        float* rawTile = getRawSlice(d);

        tiles::filters<float> filter(128);

        float min = 0.0;
        float max = 1.0;

        // TODO: use actual range in channel data
        // mpMipVolume->GetBounds(mx, mn);

        char* ret = filter.rescaleAndCast<char>(rawTile, min, max, 255.0);

        return ret;
    }

    template <typename T>
    inline T* getRawSlice(T* d) const
    {
        rawChunkSlicer<T> slicer(128, d);

        project::project::FileReadSemaphore().acquire(1);
        T* tile = slicer.GetCopyOfTile(plane_, depth_);
        project::project::FileReadSemaphore().release(1);

        return tile;
    }
};

} // namespace chunk
} // namespace om

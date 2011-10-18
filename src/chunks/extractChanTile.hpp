#pragma once

#include "chunks/rawChunkSlicer.hpp"
#include "volume/volume.h"

namespace om {
namespace chunk {

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
    boost::shared_ptr<uint32_t> Extract(T* d){
        return extractDataSlice8bit(d);
    }

private:
    template <typename T>
    boost::shared_ptr<uint32_t> extractDataSlice8bit(T* d)
    {
        boost::shared_ptr<T> rawTile = getRawSlice(d);
        boost::shared_ptr<uint32_t> ret =
            utility::smartPtr<uint32_t>::MallocNumElements(chunkSize_, common::DONT_ZERO_FILL);

        std::copy(rawTile.get(), rawTile.get() + chunkSize_, ret);
        return ret;
    }

    boost::shared_ptr<uint32_t> extractDataSlice8bit(uint32_t* d){
        return getRawSlice(d);
    }

    uint32_t* extractDataSlice8bit(float* d)
    {
        boost::shared_ptr<float> rawTile = getRawSlice(d);

        tileFilters<float> filter(128);

        float min = 0.0;
        float max = 1.0;

        // TODO: use actual range in channel data
        // mpMipVolume->GetBounds(mx, mn);

        return filter.rescaleAndCast<uint8_t>(rawTile.get(), mn, mx, 255.0);
    }

    template <typename T>
    inline boost::shared_ptr<T> getRawSlice(T* d) const
    {
        rawChunkSlicer<T> slicer(128, d);

        project::Globals().FileReadSemaphore().acquire(1);
        boost::shared_ptr<T> tile = slicer.GetCopyOfTile(plane_, depth_);
        project::Globals().FileReadSemaphore().release(1);

        return tile;
    }
};

} // namespace chunk
} // namespace om

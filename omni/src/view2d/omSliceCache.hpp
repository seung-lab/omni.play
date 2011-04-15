#ifndef OM_SLICE_CACHE_HPP
#define OM_SLICE_CACHE_HPP

#include "chunks/omSegChunk.h"
#include "system/cache/omVolSliceCache.hpp"

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

/**
 * unmanaged cache of slices to speed-up brush select tool
 *
 * NOT thread-safe
 *
 * for transient-use only
 *
 * assumes slices are in MIP 0, and are all in the same plane
 *
 */

class OmSliceCache {
private:
    OmSegmentation *const vol_;
    const ViewType viewType_;
    const int chunkDim_;

    // ignore mip level--always 0
    // x, y, z, depth
    typedef boost::tuple<int, int, int, int> OmSliceKey;

    std::map<OmSliceKey, PooledTile32Ptr> cache_;

public:
    OmSliceCache(OmSegmentation* vol, const ViewType viewType)
        : vol_(vol)
        , viewType_(viewType)
        , chunkDim_(vol->Coords().GetChunkDimension())
    {}

    OmSegID GetVoxelValue(const OmChunkCoord& chunkCoord, const Vector3i& chunkPos)
    {
        const int depth = chunkPos.z;

        PooledTile32Ptr slicePtr = GetSlice(chunkCoord, depth);

        uint32_t const*const sliceData = slicePtr->GetData();

        const uint32_t offset = chunkDim_*chunkPos.y + chunkPos.x;

        return sliceData[offset];
    }

    PooledTile32Ptr GetSlice(const OmChunkCoord& chunkCoord, const int depth)
    {
        const OmSliceKey key(chunkCoord.Coordinate.x,
                             chunkCoord.Coordinate.y,
                             chunkCoord.Coordinate.z,
                             depth);

        if(cache_.count(key)){
            return cache_[key];
        }

        OmSegChunk* chunk = vol_->GetChunk(chunkCoord);

        return cache_[key] = chunk->SegData()->ExtractDataSlice32bit(viewType_, depth);
    }
};

#endif

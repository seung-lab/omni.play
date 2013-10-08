#pragma once

#include "chunks/omSegChunk.h"
#include "tiles/cache/raw/omRawSegTileCache.hpp"
#include "chunks/omSegChunkDataInterface.hpp"

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
  OmSegmentation* const vol_;
  const ViewType viewType_;
  const int chunkDim_;

  // ignore mip level--always 0
  // x, y, z, depth
  typedef boost::tuple<int, int, int, int> OmSliceKey;

  std::map<OmSliceKey, PooledTile32Ptr> cache_;

 public:
  OmSliceCache(OmSegmentation* vol, const ViewType viewType)
      : vol_(vol),
        viewType_(viewType),
        chunkDim_(vol->Coords().GetChunkDimension()) {}

  OmSegID GetVoxelValue(const om::dataCoord& coord) {
    const int depthInChunk = coord.toTileDepth(viewType_);

    PooledTile32Ptr slicePtr = GetSlice(coord.toChunkCoord(), depthInChunk);

    uint32_t const* const sliceData = slicePtr->GetData();

    const uint32_t offset = coord.toTileOffset(viewType_);

    return sliceData[offset];
  }

  PooledTile32Ptr GetSlice(const om::chunkCoord& chunkCoord,
                           const int depthInChunk) {
    const OmSliceKey key(chunkCoord.Coordinate.x, chunkCoord.Coordinate.y,
                         chunkCoord.Coordinate.z, depthInChunk);

    if (cache_.count(key)) {
      return cache_[key];
    }

    OmSegChunk* chunk = vol_->GetChunk(chunkCoord);

    return cache_[key] =
               chunk->SegData()->ExtractDataSlice32bit(viewType_, depthInChunk);
  }
};

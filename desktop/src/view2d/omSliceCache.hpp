#pragma once
#include "precomp.h"

#include "tiles/cache/raw/omRawSegTileCache.hpp"
#include "volume/omSegmentation.h"
#include "chunk/rawChunkSlicer.hpp"

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
  const om::common::ViewType viewType_;
  const int chunkDim_;

  // ignore mip level--always 0
  // x, y, z, depth
  typedef std::tuple<int, int, int, int> OmSliceKey;

  std::map<OmSliceKey, std::shared_ptr<uint32_t>> cache_;

 public:
  OmSliceCache(OmSegmentation* vol, const om::common::ViewType viewType)
      : vol_(vol),
        viewType_(viewType),
        chunkDim_(vol->Coords().ChunkDimensions().x) {}

  om::common::SegID GetVoxelValue(const om::coords::Data& coord) {
    const int depthInChunk = coord.ToTileDepth(viewType_);

    auto slicePtr = GetSlice(coord.ToChunk(), depthInChunk);

    uint32_t const* const sliceData = slicePtr.get();

    const uint32_t offset = coord.ToTileOffset(viewType_);

    return sliceData[offset];
  }

  std::shared_ptr<uint32_t> GetSlice(const om::coords::Chunk& chunkCoord,
                                     const int depthInChunk) {
    const OmSliceKey key(chunkCoord.x, chunkCoord.y, chunkCoord.z,
                         depthInChunk);

    if (cache_.count(key)) {
      return cache_[key];
    }

    auto chunk = vol_->GetChunk(chunkCoord);
    auto typedChunk = boost::get<om::chunk::Chunk<uint32_t>>(chunk.get());
    if (!typedChunk) {
      throw om::InvalidOperationException("Unable to load chunk for slicing.");
    }
    om::chunk::rawChunkSlicer<uint32_t> slicer(chunkDim_,
                                               typedChunk->data().get());
    return cache_[key] = slicer.GetCopyOfTile(viewType_, depthInChunk);
  }
};

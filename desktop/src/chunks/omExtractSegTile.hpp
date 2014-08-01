#pragma once
#include "precomp.h"

#include "tile/tileFilters.hpp"
#include "chunks/omRawChunkSlicer.hpp"
#include "tiles/cache/raw/omRawSegTileCache.hpp"
#include "volume/omSegmentation.h"
#include "project/omProjectGlobals.h"

namespace om {
namespace segchunk {

class OmExtractSegTile {
 public:
  OmExtractSegTile(OmSegmentation* vol, const om::coords::Chunk& coord,
                   const om::common::ViewType plane, int depth)
      : vol_(vol), coord_(coord), plane_(plane), depth_(depth) {}

  template <typename T>
  inline std::shared_ptr<uint32_t> Extract(T* d) const {
    return getCachedTile(d);
  }

  std::shared_ptr<uint32_t> Extract(float*) const {
    throw om::IoException("segmentation data shouldn't be float");
  }

 private:
  template <typename T>
  std::shared_ptr<uint32_t> getCachedTile(T* d) const {
    auto dataPtr = vol_->SliceCache().Get(coord_, depth_, plane_);

    if (!dataPtr) {
      dataPtr = getTile(d);
      vol_->SliceCache().Set(coord_, depth_, plane_, dataPtr);
    }

    return dataPtr;
  }

  std::shared_ptr<uint32_t> getTile(uint32_t* d) const {
    OmRawChunkSlicer<uint32_t> slicer(128, d);

    OmProject::Globals().FileReadSemaphore().acquire(1);
    auto tile = slicer.GetCopyOfTile(plane_, depth_);
    OmProject::Globals().FileReadSemaphore().release(1);

    return tile;
  }

  template <typename T>
  std::shared_ptr<uint32_t> getTile(T* d) const {
    OmRawChunkSlicer<T> slicer(128, d);

    OmProject::Globals().FileReadSemaphore().acquire(1);
    auto rawTile = slicer.GetCopyOfTile(plane_, depth_);
    OmProject::Globals().FileReadSemaphore().release(1);

    om::tile::Filters<T> filter(128);

    return filter.recastToUint32(rawTile.get());
  }

  OmSegmentation* const vol_;
  const om::coords::Chunk coord_;
  const om::common::ViewType plane_;
  const int depth_;
};

}  // namespace segchunk
}  // namespace om

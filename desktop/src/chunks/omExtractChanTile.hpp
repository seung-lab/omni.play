#pragma once
#include "precomp.h"

#include "tiles/omTileFilters.hpp"
#include "chunks/omRawChunkSlicer.hpp"
#include "volume/omMipVolume.h"
#include "project/omProject.h"
#include "project/omProjectGlobals.h"

namespace om {
namespace chunk {

class extractChanTile {
 private:
  OmMipVolume* const vol_;
  const om::coords::Chunk coord_;
  const om::common::ViewType plane_;
  const int depth_;

 public:
  extractChanTile(OmMipVolume* vol, const om::coords::Chunk& coord,
                  const om::common::ViewType plane, int depth)
      : vol_(vol), coord_(coord), plane_(plane), depth_(depth) {}

  template <typename T>
  std::shared_ptr<uint8_t> Extract(T* d) {
    return extractDataSlice8bit(d);
  }

 private:
  template <typename T>
  std::shared_ptr<uint8_t> extractDataSlice8bit(T* d) {
    auto rawTile = getRawSlice(d);
    OmTileFilters<T> filter(128);
    return filter.recastToUint8(rawTile.get());
  }

  std::shared_ptr<uint8_t> extractDataSlice8bit(uint8_t* d) {
    return getRawSlice(d);
  }

  std::shared_ptr<uint8_t> extractDataSlice8bit(float* d) {
    std::shared_ptr<float> rawTile = getRawSlice(d);

    OmTileFilters<float> filter(128);

    float mn = 0.0;
    float mx = 1.0;

    // TODO: use actual range in channel data
    // mpMipVolume->GetBounds(mx, mn);

    return filter.rescaleAndCast<uint8_t>(rawTile.get(), mn, mx, 255.0);
  }

  template <typename T>
  inline std::shared_ptr<T> getRawSlice(T* d) const {
    OmRawChunkSlicer<T> slicer(128, d);

    OmProject::Globals().FileReadSemaphore().acquire(1);
    auto tile = slicer.GetCopyOfTile(plane_, depth_);
    OmProject::Globals().FileReadSemaphore().release(1);

    return tile;
  }
};

}  // namespace chunk
}  // namespace om

#pragma once

#include "utility/malloc.hpp"

namespace om {
namespace chunks {

// TODO: refactor for non-square chunk dims
template <typename T> class rawChunkSlicer {
 private:
  const int chunkDim_;         // usually 128
  const int elementsPerTile_;  // chunkDim^2

  T const* const chunkPtr_;

 public:
  rawChunkSlicer(const int chunkDim, T const* const chunkPtr)
      : chunkDim_(chunkDim),
        elementsPerTile_(chunkDim * chunkDim),
        chunkPtr_(chunkPtr) {}

  std::shared_ptr<T> GetCopyOfTile(const om::common::ViewType viewType,
                                   const int offsetNumTiles) {
    auto tilePtr =
        mem::Malloc<T>::NumElements(elementsPerTile_, mem::ZeroFill::DONT);
    sliceTile(viewType, offsetNumTiles, tilePtr.get());
    return tilePtr;
  }

 private:
  void sliceTile(const om::common::ViewType viewType, const int offsetNumTiles,
                 T* tile) {
    switch (viewType) {

      case common::ViewType::XY_VIEW: {
        // skip to requested XY plane, then copy entire plane
        T const* const start = chunkPtr_ + offsetNumTiles * elementsPerTile_;
        T const* const end = start + elementsPerTile_;
        std::copy(start, end, tile);
      } break;

      case common::ViewType::XZ_VIEW: {
        // skip to first scanline, then copy every scanline in XZ plane
        T const* start = chunkPtr_ + chunkDim_ * offsetNumTiles;
        for (auto i = 0; i < elementsPerTile_; i += chunkDim_) {
          std::copy(start, start + chunkDim_, &tile[i]);
          start += elementsPerTile_;
        }
      } break;

      case common::ViewType::ZY_VIEW: {
        // skip to first voxel in plane, then copy every voxel,
        //   advancing each time to the next scanline
        T const* start = chunkPtr_ + offsetNumTiles;
        for (auto i = 0; i < chunkDim_; ++i) {
          for (auto j = 0; j < chunkDim_; ++j) {
            tile[j * chunkDim_ + i] = *start;
            start += chunkDim_;
          }
        }
      } break;

      default:
        throw ArgException("unknown plane");
    }
  }
};

}  // namespace chunks
}  // namespace om

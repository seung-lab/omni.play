#pragma once

#include "utility/malloc.hpp"

namespace om {
namespace chunk {

// TODO: refactor for non-square chunk dims
template <typename T>
class rawChunkSlicer {
 private:
  const int chunkDim_;         // usually 128
  const int elementsPerTile_;  // chunkDim^2

  T* const chunkPtr_;

 public:
  rawChunkSlicer(const int chunkDim, T* chunkPtr)
      : chunkDim_(chunkDim),
        elementsPerTile_(chunkDim * chunkDim),
        chunkPtr_(chunkPtr) {}

  std::shared_ptr<T> GetCopyOfTile(const om::common::ViewType viewType,
                                   const int offsetNumTiles) {
    assert(offsetNumTiles < chunkDim_);
    auto tilePtr =
        mem::Malloc<T>::NumElements(elementsPerTile_, mem::ZeroFill::DONT);
    sliceTile(viewType, offsetNumTiles, tilePtr.get());
    return tilePtr;
  }

  void WriteTileToChunk(const om::common::ViewType viewType,
                        const int offsetNumTiles, T* tile) {
    if (!tile) {
      return;
    }
    switch (viewType) {
      case common::ViewType::XY_VIEW: {
        // skip to requested XY plane, then copy entire plane
        T* start = chunkPtr_ + offsetNumTiles * elementsPerTile_;
        T* end = tile + elementsPerTile_;
        std::copy(tile, end, start);
      } break;

      case common::ViewType::XZ_VIEW: {
        // skip to first scanline, then copy every scanline in XZ plane
        T* start = chunkPtr_ + chunkDim_ * offsetNumTiles;
        for (auto i = 0; i < elementsPerTile_; i += chunkDim_) {
          std::copy(&tile[i], &tile[i] + chunkDim_, start);
          start += elementsPerTile_;
        }
      } break;

      case common::ViewType::ZY_VIEW: {
        // skip to first voxel in plane, then copy every voxel,
        //   advancing each time to the next scanline
        T* start = chunkPtr_ + offsetNumTiles;
        for (auto i = 0; i < chunkDim_; ++i) {
          for (auto j = 0; j < chunkDim_; ++j) {
            *start = tile[j * chunkDim_ + i];
            start += chunkDim_;
          }
        }
      } break;
    }
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
    }
  }
};

extern template class rawChunkSlicer<int8_t>;
extern template class rawChunkSlicer<uint8_t>;
extern template class rawChunkSlicer<int32_t>;
extern template class rawChunkSlicer<uint32_t>;
extern template class rawChunkSlicer<float>;

}  // namespace chunks
}  // namespace om

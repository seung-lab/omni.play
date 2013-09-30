#pragma once

#include "project/omProject.h"
#include "project/omProjectGlobals.h"
#include "tiles/pools/omPooledTile.hpp"
#include "tiles/pools/omTilePool.hpp"
#include "utility/malloc.hpp"

template <typename T> class OmRawChunkSlicer {
 private:
  const int chunkDim_;         // usually 128
  const int elementsPerTile_;  // chunkDim^2

  T const* const chunkPtr_;

 public:
  OmRawChunkSlicer(const int chunkDim, T const* const chunkPtr)
      : chunkDim_(chunkDim),
        elementsPerTile_(chunkDim * chunkDim),
        chunkPtr_(chunkPtr) {}

  OmPooledTile<T>* GetCopyAsPooledTile(const om::common::ViewType viewType,
                                       const int offsetNumTiles) {
    OmPooledTile<T>* pooledTile = new OmPooledTile<T>();

    sliceTile(viewType, offsetNumTiles, pooledTile->GetData());

    return pooledTile;
  }

  std::shared_ptr<T> GetCopyOfTile(const om::common::ViewType viewType,
                                   const int offsetNumTiles) {
    std::shared_ptr<T> tilePtr = om::mem::Malloc<T>::NumElements(
        elementsPerTile_, om::mem::ZeroFill::DONT);
    sliceTile(viewType, offsetNumTiles, tilePtr.get());

    return tilePtr;
  }

 private:
  void sliceTile(const om::common::ViewType viewType, const int offsetNumTiles,
                 T* tile) {
    switch (viewType) {

      case om::common::XY_VIEW: {
        // skip to requested XY plane, then copy entire plane
        T const* const start = chunkPtr_ + offsetNumTiles * elementsPerTile_;
        T const* const end = start + elementsPerTile_;
        std::copy(start, end, tile);
      } break;

      case om::common::XZ_VIEW: {
        // skip to first scanline, then copy every scanline in XZ plane
        T const* start = chunkPtr_ + chunkDim_ * offsetNumTiles;
        for (int i = 0; i < elementsPerTile_; i += chunkDim_) {
          std::copy(start, start + chunkDim_, &tile[i]);
          start += elementsPerTile_;
        }
      } break;

      case om::common::ZY_VIEW: {
        // skip to first voxel in plane, then copy every voxel,
        //   advancing each time to the next scanline
        T const* start = chunkPtr_ + offsetNumTiles;
        for (int i = 0; i < chunkDim_; ++i) {
          for (int j = 0; j < chunkDim_; ++j) {
            tile[j * chunkDim_ + i] = *start;
            start += chunkDim_;
          }
        }
      } break;

      default:
        throw om::ArgException("unknown plane");
    }
    ;
  }
};

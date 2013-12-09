#pragma once

#include "pipeline/stage.hpp"
#include "chunk/rawChunkSlicer.hpp"
#include "datalayer/memMappedFile.hpp"
#include "tile/tile.hpp"

namespace om {
namespace pipeline {

class getTile : public stage {
 public:
  template <typename T> data_var operator()(const tile::Tile<T>& in) const {
    return Data<T>(in.data(), in.length());
  }
};

inline data_var operator>>(const tile::TileVar& d, const getTile& v) {
  return boost::apply_visitor(v, d);
}

class TileSlicer
    : public boost::static_visitor<std::shared_ptr<tile::TileVar>> {
 public:
  TileSlicer(coords::Tile coord, coords::VolumeSystem system)
      : coord_(coord), system_(system) {}

  template <typename T>
  std::shared_ptr<tile::TileVar> operator()(
      const datalayer::MemMappedFile<T>& in) const {
    return std::make_shared<tile::TileVar>(
        tile::Tile<T>(coord_, system_, slice<T>(in, coord_, system_)));
  }

  template <typename T>
  std::shared_ptr<T> slice(const datalayer::MemMappedFile<T>& in,
                           coords::Tile coord,
                           const coords::VolumeSystem& system) const {
    if (!in.IsMapped()) {
      throw ArgException("Invalid volume: " + in.GetBaseFileName().string());
    }
    int chunkSize = system.ChunkDimensions().x;
    uint64_t offset = coord.chunk().PtrOffset(system, sizeof(T));
    T* chunkPtr = in.GetPtrWithOffset(offset);
    chunks::rawChunkSlicer<T> slicer(chunkSize, chunkPtr);
    return slicer.GetCopyOfTile(coord.viewType(), coord.depth());
  }

 private:
  coords::Tile coord_;
  coords::VolumeSystem system_;
};

inline std::shared_ptr<tile::TileVar> operator>>(const dataSrcs& d,
                                                 const TileSlicer& v) {
  return boost::apply_visitor(v, d);
}
}
}

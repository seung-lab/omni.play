#pragma once

#include "tile/dataSource.h"
#include "tile/slicerDataSource.h"
#include "datalayer/dataSourceHierarchy.hpp"
#include "datalayer/cacheDataSource.hpp"

namespace om {
namespace tile {

class CachedDataSource : public tile::TileDSH {
 public:
  CachedDataSource(const file::path& path, const common::DataType& type,
                   const coords::VolumeSystem& system)
      : tile::TileDSH(),
        toCache_(10000),
        system_(system) {  // 10000 * 64kb = 625MB

    Register<FileDataSource>(0, path, type, system);
    Register<datalayer::CacheDataSource<coords::Tile, tile::TileVar>>(0,
                                                                      toCache_);
  }

  CachedDataSource(chunk::ChunkDS& ds, const coords::VolumeSystem& system)
      : tile::TileDSH(),
        toCache_(10000),
        system_(system) {  // 10000 * 64kb = 625MB

    Register<SlicerDataSource>(0, ds, system);
    Register<datalayer::CacheDataSource<coords::Tile, tile::TileVar>>(0,
                                                                      toCache_);
  }

  using tile::TileDSH::Invalidate;
  void Invalidate(const coords::Chunk& chunk) {
    auto max = chunk.BoundingBox(system_).getMax();
    for (int i = 0; i < max.ToChunkVec().x; ++i) {
      Invalidate(coords::Tile(chunk, common::ViewType::ZY_VIEW, i));
    }
    for (int i = 0; i < max.ToChunkVec().y; ++i) {
      Invalidate(coords::Tile(chunk, common::ViewType::XZ_VIEW, i));
    }
    for (int i = 0; i < max.ToChunkVec().z; ++i) {
      Invalidate(coords::Tile(chunk, common::ViewType::XY_VIEW, i));
    }
  }

  virtual ~CachedDataSource() {}

 protected:
  const size_t toCache_;  // TODO: Make dynamic (Cache Manager)
  const coords::VolumeSystem& system_;
};
}
}  // namespace om::tile::

#pragma once

#include "tile/dataSource.h"
#include "datalayer/dataSourceHierarchy.hpp"
#include "datalayer/cacheDataSource.hpp"

namespace om {
namespace tile {

class CachedDataSource : public tile::TileDSH {
 private:
  const size_t toCache_;  // TODO: Make dynamic (Cache Manager)

 public:
  CachedDataSource(const file::path& path, const common::DataType& type,
                   const coords::VolumeSystem& system)
      : tile::TileDSH(), toCache_(10000) {  // 10000 * 64kb = 625MB

    Register<FileDataSource>(0, path, type, system);
    Register<datalayer::CacheDataSource<coords::Tile, tile::TileVar>>(0,
                                                                      toCache_);
  }

  virtual ~CachedDataSource() {}
};
}
}  // namespace om::tile::

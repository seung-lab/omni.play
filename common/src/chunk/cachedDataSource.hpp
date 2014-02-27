#pragma once
#include "chunk/dataSource.h"
#include "datalayer/dataSourceHierarchy.hpp"
#include "datalayer/cacheDataSource.hpp"

namespace om {
namespace chunk {

class CachedDataSource : public om::chunk::ChunkDSH {
 private:
  const size_t toCache_;  // TODO: Make dynamic (Cache Manager)

 public:
  CachedDataSource(const file::path& path, const common::DataType& type,
                   const coords::VolumeSystem& system)
      : om::chunk::ChunkDSH(), toCache_(25) {  // 25 * 8MB = 200MB

    Register<FileDataSource>(0, path, type, system);
    Register<datalayer::CacheDataSource<coords::Chunk, chunk::ChunkVar>>(
        0, toCache_);
  }

  virtual ~CachedDataSource() {}
};
}
}  // namespace om::chunk::

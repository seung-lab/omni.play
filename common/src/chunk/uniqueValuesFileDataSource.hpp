#pragma once

#include "precomp.h"
#include "datalayer/vectorFileDataSource.hpp"
#include "datalayer/paths.hpp"
#include "chunk/dataSources.hpp"

namespace om {
namespace chunk {

class UniqueValuesFileDataSource
    : public datalayer::VectorFileDataSource<coords::Chunk, uint32_t> {
 public:
  UniqueValuesFileDataSource(file::path volumeRoot)
      : VectorFileDataSource(volumeRoot) {}

 protected:
  virtual file::path relativePath(const coords::Chunk& key) const override {
    return file::Paths::Seg::ChunkUniqueValues(key);
  }
};
}
}  // namespace om::chunk::

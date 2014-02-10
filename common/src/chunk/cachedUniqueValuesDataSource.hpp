#pragma once

#include "chunk/dataSources.hpp"
#include "datalayer/dataSourceHierarchy.hpp"
#include "datalayer/cacheDataSource.hpp"
#include "chunk/uniqueValuesFileDataSource.hpp"

namespace om {
namespace chunk {

class CachedUniqueValuesDataSource : public UniqueValuesDSH {
 private:
  const size_t toCache_;  // TODO: Make dynamic (Cache Manager)

 public:
  CachedUniqueValuesDataSource(file::path volPath)
      : chunk::UniqueValuesDSH(), toCache_(50) {
    Register<UniqueValuesFileDataSource>(0, volPath);
    Register<datalayer::CacheDataSource<coords::Chunk, chunk::UniqueValues>>(
        0, toCache_);
  }

  virtual ~CachedUniqueValuesDataSource() {}
};
}
}  // namespace om::chunk::

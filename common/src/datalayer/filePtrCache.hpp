#pragma once

#include "common/common.h"
#include "cache/lockedObjects.hpp"
#include "datalayer/memMappedFile.hpp"
#include "pipeline/stage.hpp"
#include "pipeline/mapData.hpp"

#include <sstream>

namespace om {
namespace datalayer {

template <typename T>
class FilePtrCache {
 public:
  FilePtrCache(size_t cacheSize = 50) : cacheSize_(cacheSize) {}

  MemMappedFile<T> Get(const std::string& fnp) {
    MemMappedFile<T> ret;
    if (!cache_.assignIfHadKey(fnp, ret)) {
      if (!file::exists(fnp)) {
        log_errors(io) << "Unable to map: " << fnp;
        return ret;
      }

      ret = MemMappedFile<T>(fnp);

      cache_.set(fnp, ret);
      while (cache_.size() > cacheSize_) {
        cache_.remove_oldest();
      }
    }
    return ret;
  }

  void Invalidate(const std::string& fnp) { cache_.erase(fnp); }

 private:
  size_t cacheSize_;
  cache::LockedCacheMap<std::string, datalayer::MemMappedFile<T>> cache_;
};

class VarFilePtrCache {
 public:
  VarFilePtrCache(size_t cacheSize = 50) : cacheSize_(cacheSize) {}

  pipeline::dataSrcs Get(const std::string& fnp,
                         const common::DataType& dataType) {
    pipeline::dataSrcs ret;
    if (!cache_.assignIfHadKey(fnp, ret)) {
      if (!file::exists(fnp)) {
        return ret;
      }

      pipeline::mapData mapped(fnp, dataType);
      ret = mapped.file();

      cache_.set(fnp, ret);
      while (cache_.size() > cacheSize_) {
        cache_.remove_oldest();
      }
    }
    return ret;
  }

  void Invalidate(const std::string& fnp) { cache_.erase(fnp); }

 private:
  size_t cacheSize_;
  cache::LockedCacheMap<std::string, pipeline::dataSrcs> cache_;
};
}
}  // namespace om::datalayer::

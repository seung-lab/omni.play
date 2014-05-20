#pragma once

#include "datalayer/dataSource.hpp"
#include "cache/lockedObjects.hpp"

namespace om {
namespace datalayer {

template <typename TKey, typename TValue>
class CacheDataSource : public IDataSource<TKey, TValue> {
 public:
  CacheDataSource(size_t cacheSize) : cacheSize_(cacheSize) {}

  std::shared_ptr<TValue> Get(const TKey& key, bool async = false) const {
    // TODO: don't return optional of shared_ptr
    boost::optional<std::shared_ptr<TValue>> data = cache_.get(key.keyStr());
    if (data) {
      return data.get();
    } else {
      return std::shared_ptr<TValue>();
    }
  }

  bool Put(const TKey& key, std::shared_ptr<TValue> value, bool async = false) {
    cache_.set(key.keyStr(), value);

    cleanCache();
    return true;
  }

  bool is_cache() const { return true; }
  bool is_persisted() const { return false; }

 protected:
  PROP_REF(size_t, cacheSize);
  mutable cache::LockedCacheMap<std::string, std::shared_ptr<TValue>> cache_;

  void cleanCache() {
    while (cache_.size() > cacheSize_) {
      cache_.remove_oldest();
    }
  }
};
}
}  // namespace om::datalayer::

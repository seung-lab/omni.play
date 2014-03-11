#pragma once

#include "datalayer/dataSource.hpp"
#include "cache/lockedObjects.hpp"
#include <atomic>
#include <future>
#include <chrono>
#include <condition_variable>
#include <mutex>

namespace om {
template <typename T>
static size_t size(const std::shared_ptr<T>& val) {
  return sizeof(T);
}

template <typename T>
static std::string key(const T& key) {
  return std::to_string(key);
}

namespace datalayer {

template <typename TKey, typename TValue>
class SizedCacheDataSource : public IDataSource<TKey, TValue> {
 public:
  SizedCacheDataSource(size_t maxBytes)
      : bytes_(0), maxBytes_(maxBytes), killing_(false) {
    cleaner_ = std::async(std::launch::async,
                          std::bind(&SizedCacheDataSource::cleanCache, this));
  }

  ~SizedCacheDataSource() {
    killing_.store(true);
    cv_.notify_all();
  }

  std::shared_ptr<TValue> Get(const TKey& key, bool async = false) {
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

    std::atomic_fetch_add(&bytes_, om::size(value));
    return true;
  }

  bool is_cache() const { return true; }
  bool is_persisted() const { return false; }

  size_t bytes() const { return bytes_.load(); }

 protected:
  std::atomic<size_t> bytes_;
  PROP_CONST_REF(size_t, maxBytes);
  cache::LockedCacheMap<std::string, std::shared_ptr<TValue>> cache_;
  std::future<void> cleaner_;
  std::atomic<bool> killing_;
  std::mutex m_;
  std::condition_variable cv_;

  void cleanCache() {
    std::unique_lock<std::mutex> l(m_);

    while (!cv_.wait_for(l, std::chrono::milliseconds(1000),
                         [this]() { return killing_.load(); })) {
      while (bytes_.load() > maxBytes_) {
        auto oldest = cache_.remove_oldest();
        if (oldest) {
          std::atomic_fetch_sub(&bytes_, om::size(oldest.get()));
        }
      }
    }
  }
};
}
}  // namespace om::datalayer::

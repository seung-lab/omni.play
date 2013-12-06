#pragma once

#include "datalayer/dataSource.hpp"
#include "common/container.hpp"
#include "common/enums.hpp"
#include "threads/taskManagerTypes.h"
#include "threads/taskManager.hpp"
#include "zi/concurrency/runnable.hpp"
#include "cache/lockedObjects.hpp"
#include <atomic>

namespace om {
namespace datalayer {

template <typename TKey, typename TValue>
class DataSourceHierarchy : public IDataSource<TKey, TValue> {
 public:
  DataSourceHierarchy() : DataSourceHierarchy(1) {}
  DataSourceHierarchy(uint threads)
      : numThreads_(threads), killingCache_(false) {
    threadPool_.start(threads);
  }

  virtual ~DataSourceHierarchy() {
    killingCache_.store(true);
    threadPool_.stop();
    // currentlyFetching_.clear();
  }

  std::shared_ptr<TValue> Get(const TKey& key, bool async = false) {
    std::shared_ptr<TValue> ret;

    // Try getting from cache.
    for (auto& s : sources_) {
      if (s->is_cache()) {
        ret = s->Get(key);
        if (ret) {
          return ret;
        }
      }
    }

    if (async) {
      // Don't start too many or double queue.  It's okay to fail.
      if (threadPool_.getTaskCount() < numThreads_) {
        // if (!currentlyFetching_.insertSinceDidNotHaveKey(key)) {
        //   log_debugs << "Already Getting " << key;
        //   return std::shared_ptr<TValue>();
        // }
        threadPool_.push_back(
            std::bind(&DataSourceHierarchy::do_get, this, key));
      }
    } else {
      return do_get(key);
    }

    return std::shared_ptr<TValue>();
  }

  bool Put(const TKey& key, std::shared_ptr<TValue> data, bool async = false) {
    if (async) {
      threadPool_.push_front(
          std::bind(&DataSourceHierarchy::store, this, key, data, true));
    } else {
      store(key, data, true);
    }
    return true;
  }

  template <typename T, typename... ARGS>
  void Register(size_t priority, ARGS&&... args) {
    if (priority >= sources_.size()) {
      sources_.emplace_back(std::make_unique<T>(args...));
    } else {
      sources_.emplace(sources_.begin() + priority,
                       std::make_unique<T>(args...));
    }
  }

  void Flush() {
    for (auto& s : sources_) {
      if (s->is_persisted()) {
        s->Flush();
      }
    }
  }

  bool is_cache() const {
    for (auto& s : sources_) {
      if (s->is_cache()) {
        return true;
      }
    }
    return false;
  }

  bool is_persisted() const {
    for (auto& s : sources_) {
      if (s->is_persisted()) {
        return true;
      }
    }
    return false;
  }

 protected:
  void store(const TKey& key, std::shared_ptr<TValue> data, bool persist) {
    for (auto& s : sources_) {
      if (s->is_cache()) {
        s->Put(key, data);
      } else if (persist && s->is_persisted()) {
        s->Put(key, data);
        return;
      }
    }
  }

  std::shared_ptr<TValue> do_get(const TKey& key) {
    if (killingCache_.load()) {
      return std::shared_ptr<TValue>();
    }
    std::shared_ptr<TValue> ret;

    for (auto& s : sources_) {
      ret = s->Get(key);
      if (ret) {
        store(key, ret, false);
        // currentlyFetching_.erase(key);
        return ret;
      }
    }

    // currentlyFetching_.erase(key);
    return std::shared_ptr<TValue>();
  }

  void do_store(const std::pair<TKey, std::shared_ptr<TValue>>& toStore) {
    store(toStore.first, toStore.second, true);
  }

  // TODO: Lock access to this Vector
  std::vector<std::unique_ptr<IDataSource<TKey, TValue>>> sources_;
  uint numThreads_;
  thread::ThreadPool threadPool_;
  // cache::LockedKeySet<TKey> currentlyFetching_;
  std::atomic_bool killingCache_;
};
}
}  // namespace om::datalayer::

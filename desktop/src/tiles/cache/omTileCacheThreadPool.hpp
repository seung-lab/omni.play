#pragma once
#include "precomp.h"

#include "threads/omTaskManager.hpp"
#include "tiles/cache/omTaskManagerContainerMipSorted.hpp"

#include "tiles/omTileCoord.h"

class OmTileCacheThreadPool {
 private:
  typedef OmTileCoord key_t;

  std::set<key_t> currentlyFetching_;
  typedef std::set<key_t>::iterator iterator;

  OmTaskManager<OmTaskManagerContainerMipSorted> threadPool_;

  zi::spinlock lock_;

 public:
  void start() { threadPool_.start(); }

  void clear() {
    zi::guard g(lock_);
    currentlyFetching_.clear();
    threadPool_.clear();
  }

  void stop() {
    zi::guard g(lock_);
    currentlyFetching_.clear();
    threadPool_.stop();
  }

  template <typename PtrToMemFunc, typename KlassInstance>
  void QueueUpIfKeyNotPresent(const key_t& key, PtrToMemFunc ptrMemFunc,
                              KlassInstance klassInstance) {
    zi::guard g(lock_);

    if (insertSinceDidNotHaveKey(key)) {
      threadPool_.insert(key.getCoord().mipLevel(),
                         zi::run_fn(zi::bind(ptrMemFunc, klassInstance, key)));
    }
  }

 private:
  inline bool insertSinceDidNotHaveKey(const key_t& k) {
    std::pair<iterator, bool> p = currentlyFetching_.insert(k);
    return p.second;
  }
};

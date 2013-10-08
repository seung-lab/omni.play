#pragma once

#include "common/om.hpp"
#include "system/cache/omCacheBase.h"
#include "system/cache/omCacheManager.h"
#include "system/cache/omLockedCacheObjects.hpp"
#include "utility/omLockedObjects.h"
#include "utility/omLockedPODs.hpp"
#include "threads/omTaskManager.hpp"
#include "zi/omMutex.h"

#include <zi/system.hpp>

/**
 *  Brett Warne - bwarne@mit.edu - 3/12/09
 */

class OmThreadedMeshCache : public OmCacheBase {
 private:
  typedef OmMeshCoord key_t;
  typedef OmMeshPtr ptr_t;

  OmThreadPool threadPool_;
  LockedInt64 curSize_;

  LockedCacheMap<key_t, ptr_t> cache_;
  LockedKeySet<key_t> currentlyFetching_;
  LockedBool killingCache_;

  struct OldCache {
    std::map<key_t, ptr_t> map;
    KeyMultiIndex<key_t> list;
  };
  LockedList<om::shared_ptr<OldCache> > cachesToClean_;

  int numThreads() { return 2; }

 public:
  OmThreadedMeshCache(const om::CacheGroup group, const std::string& name)
      : OmCacheBase(name, group) {
    OmCacheManager::AddCache(group, this);
    threadPool_.start(numThreads());
  }

  virtual ~OmThreadedMeshCache() {
    CloseDownThreads();
    OmCacheManager::RemoveCache(cacheGroup_, this);
  }

  virtual void Get(ptr_t& ptr, const key_t& key, const om::Blocking blocking) {
    if (cache_.assignIfHadKey(key, ptr)) {
      return;
    }

    if (om::BLOCKING == blocking) {
      ptr = loadItem(key);
      return;
    }

    if (zi::system::cpu_count == threadPool_.getTaskCount()) {
      return;  // restrict number of tasks to process
    }

    if (currentlyFetching_.insertSinceDidNotHaveKey(key)) {
      threadPool_.push_back(zi::run_fn(
          zi::bind(&OmThreadedMeshCache::handleCacheMissThread, this, key)));
    }
  }

  virtual ptr_t HandleCacheMiss(const key_t& key) = 0;

  void Remove(const key_t& key) {
    ptr_t ptr = cache_.erase(key);
    if (!ptr) {
      return;
    }
    curSize_.sub(ptr->NumBytes());
  }

  void RemoveOldest(const int64_t numBytesToRemove) {
    int64_t numBytesRemoved = 0;

    while (numBytesRemoved < numBytesToRemove) {
      if (cache_.empty()) {
        return;
      }

      const ptr_t ptr = cache_.remove_oldest();

      if (!ptr) {
        continue;
      }

      const int64_t removedBytes = ptr->NumBytes();
      numBytesRemoved += removedBytes;
      curSize_.sub(removedBytes);
    }
  }

  void Clean() {
    if (cachesToClean_.empty()) {
      return;
    }

    // avoid contention on cacheToClean by swapping in new, empty list
    std::list<om::shared_ptr<OldCache> > oldCaches;
    cachesToClean_.swap(oldCaches);
  }

  void Clear() {
    cache_.clear();
    currentlyFetching_.clear();
    curSize_.set(0);
  }

  void ClearFetchQueue() {
    threadPool_.clear();
    currentlyFetching_.clear();
  }

  void InvalidateCache() {
    ClearFetchQueue();

    // add current cache to list to be cleaned later by OmCacheMan thread
    om::shared_ptr<OldCache> cache(new OldCache());

    cache_.swap(cache->map, cache->list);
    cachesToClean_.push_back(cache);

    curSize_.set(0);
  }

  int64_t GetCacheSize() const { return curSize_.get(); }

  void CloseDownThreads() {
    killingCache_.set(true);
    threadPool_.stop();
    cache_.clear();
    currentlyFetching_.clear();
  }

 private:
  void handleCacheMissThread(const key_t key) {
    if (killingCache_.get()) {
      return;
    }

    loadItem(key);
  }

  ptr_t loadItem(const key_t& key) {
    ptr_t ptr = HandleCacheMiss(key);
    const bool wasInserted = cache_.set(key, ptr);
    if (wasInserted) {
      curSize_.add(ptr->NumBytes());
    }
    return ptr;
  }
};

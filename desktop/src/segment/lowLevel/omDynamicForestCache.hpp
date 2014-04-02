#pragma once
#include "precomp.h"

#include "common/common.h"
#include "segment/lowLevel/DynamicForestPool.hpp"

class OmDynamicForestCache {
 private:
  std::unique_ptr<zi::DynamicForestPool<om::common::SegID>> graph_;

  std::atomic<uint64_t> freshness_;
  zi::spinlock lock_;

  bool batchMode_;

  inline void clearCacheIfNotBatch() {
    if (batchMode_) {
      return;
    }

    ClearCache();
  }

 public:
  OmDynamicForestCache(const size_t size)
      : graph_(new zi::DynamicForestPool<om::common::SegID>(size)),
        freshness_(1),
        batchMode_(false) {}

  ~OmDynamicForestCache() {}

  inline uint64_t Freshness() const { return freshness_.load(); }

  inline bool Batch() const { return batchMode_; }

  inline void SetBatch(const bool batchModeOn) {
    ClearCache();
    batchMode_ = batchModeOn;
  }

  inline void ClearCache() { ++freshness_; }

  inline void Cut(const om::common::SegID segID) {
    zi::guard g(lock_);
    clearCacheIfNotBatch();
    graph_->Cut(segID);
  }

  inline om::common::SegID Root(const om::common::SegID segID) {
    zi::guard g(lock_);
    return graph_->Root(segID);
  }

  inline void Join(const om::common::SegID childRootID,
                   const om::common::SegID parentRootID) {
    zi::guard g(lock_);
    clearCacheIfNotBatch();
    graph_->Join(childRootID, parentRootID);
  }

  inline size_t Size() const { return graph_->Size(); }

  inline void Resize(const size_t size) {
    zi::guard g(lock_);
    graph_->Resize(size);
  }
};

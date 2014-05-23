#pragma once

#include "segment/lowLevel/omDynamicForestCache.hpp"

namespace om {
namespace segment {

class CacheRootIDs {
 public:
  CacheRootIDs(OmDynamicForestCache* graph, size_t maxSegments)
      : graph_(graph), cache_(maxSegments) {}

  common::SegID Root(const common::SegID segID, const uint64_t mstFreshness) {
    if (segID >= cache_.size()) {
      return graph_->Root(segID);
    }

    {
      zi::spinlock::pool<cache_root_segment_id_initial_tag>::guard g(segID);
      if (cache_[segID].freshness == mstFreshness) {
        return cache_[segID].segID;
      }
    }

    auto rootSegID = graph_->Root(segID);
    {
      zi::spinlock::pool<cache_root_segment_id_initial_tag>::guard g(segID);
      cache_[segID].freshness = mstFreshness;
      cache_[segID].segID = rootSegID;
    }
    return rootSegID;
  }

 private:
  OmDynamicForestCache* graph_;

  struct RootAndMSTFreshnes {
    uint64_t freshness;
    uint32_t segID;
  };

  std::vector<RootAndMSTFreshnes> cache_;
  struct cache_root_segment_id_initial_tag;
};
}
}  // namespace om::segment::

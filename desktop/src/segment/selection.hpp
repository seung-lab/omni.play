#pragma once

#include "segment/lowLevel/graph.h"
#include "segment/lowLevel/store.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegment.h"
#include "system/cache/omCacheManager.h"

namespace om {
namespace segment {

class Selection {
 public:
  Selection(Graph& graph, Store& store, OmSegmentLists& lists)
      : graph_(graph), store_(store), lists_(lists) {}

  inline bool AreSegmentsSelected() const {
    zi::guard g(mutex_);
    return !selected_.empty();
  }

  inline uint32_t NumberOfSelectedSegments() const {
    zi::guard g(mutex_);
    return selected_.size();
  }

  inline const common::SegIDSet GetSelectedSegmentIDs() const {
    zi::guard g(mutex_);
    return selected_;
  }

  inline bool IsSegmentSelected(const common::SegID segID) const {
    zi::guard g(mutex_);
    if (selected_.empty()) {
      return false;
    }
    return selected_.count(graph_.Root(segID));
  }

  inline bool IsSegmentSelected(const OmSegment* seg) const {
    return IsSegmentSelected(seg->value());
  }

  inline void setSegmentSelected(common::SegID segID, const bool isSelected,
                                 const bool addToRecentList) {
    zi::guard g(mutex_);
    setSegmentSelectedBatch(segID, isSelected, addToRecentList);
    OmCacheManager::TouchFreshness();
  }

  void UpdateSegmentSelection(const common::SegIDSet& ids,
                              const bool addToRecentList) {
    zi::guard g(mutex_);
    selected_.clear();

    for (auto id : ids) {
      setSegmentSelectedBatch(id, true, addToRecentList);
    }

    OmCacheManager::TouchFreshness();
  }

  void AddToSegmentSelection(const common::SegIDSet& ids) {
    zi::guard g(mutex_);
    for (auto id : ids) {
      setSegmentSelectedBatch(id, true, true);
    }

    OmCacheManager::TouchFreshness();
  }

  void RemoveFromSegmentSelection(const common::SegIDSet& ids) {
    zi::guard g(mutex_);
    for (auto id : ids) {
      setSegmentSelectedBatch(id, false, false);
    }

    OmCacheManager::TouchFreshness();
  }

  void RerootSegmentList() {
    zi::guard g(mutex_);
    common::SegIDSet old = selected_;
    selected_.clear();

    for (auto id : old) {
      selected_.insert(graph_.Root(id));
    }
  }

  inline void Clear() { selected_.clear(); }

 private:
  zi::mutex mutex_;
  Graph& graph_;
  Store& store_;
  OmSegmentLists& lists_;

  common::SegIDSet selected_;

  inline void setSegmentSelectedBatch(const common::SegID segID,
                                      const bool isSelected,
                                      const bool addToRecentList) {
    const common::SegID rootID = graph_.Root(segID);

    if (isSelected) {
      doSelectedSetInsert(rootID, addToRecentList);
    } else {
      doSelectedSetRemove(rootID);
    }
  }

  inline void doSelectedSetInsert(const common::SegID segID,
                                  const bool addToRecentList) {
    selected_.insert(segID);
    if (addToRecentList) {
      addToRecentMap(segID);
    }
  }

  inline void doSelectedSetRemove(const common::SegID segID) {
    selected_.erase(segID);
    addToRecentMap(segID);
  }

  inline void addToRecentMap(const common::SegID segID) {
    OmSegment* seg = store_.GetSegment(segID);
    lists_.TouchRecent(seg);
  }
};
}
}

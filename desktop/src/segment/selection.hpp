#pragma once
#include "precomp.h"

#include "segment/lowLevel/graph.h"
#include "segment/lowLevel/store.h"
#include "segment/lists/omSegmentLists.h"
#include "segment/omSegment.h"
#include "system/cache/omCacheManager.h"
#include "datalayer/archive/segmentation.h"

class OmSegmentsImpl;

namespace om {
namespace segment {

class Selection {
 public:
  Selection(Graph& graph, Store& store, OmSegmentLists& lists)
      : graph_(graph), store_(store), lists_(lists) {}

  inline bool AreSegmentsSelected() const {
    zi::guard g(mutex_);
    return !selectedIDsToOrders_.empty();
  }

  inline uint32_t NumberOfSelectedSegments() const {
    zi::guard g(mutex_);
    return selectedIDsToOrders_.size();
  }

  inline const common::SegIDMap GetSelectedSegmentIDsWithOrder() const {
    zi::guard g(mutex_);

    return selectedIDsToOrders_;
  }

  inline const common::SegIDSet GetSelectedSegmentIDs() const {
    zi::guard g(mutex_);
    common::SegIDSet selectedSet;
    boost::copy(selectedIDsToOrders_ | boost::adaptors::map_keys,
                  std::inserter(selectedSet, selectedSet.begin()));
    return selectedSet;
  }

  inline bool IsSegmentSelected(const common::SegID segID) const {
    zi::guard g(mutex_);
    if (selectedIDsToOrders_.empty()) {
      return false;
    }
    return selectedIDsToOrders_.count(graph_.Root(segID));
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

  void UpdateSegmentSelection(const common::SegIDMap& segIDToOrders,
                              const bool shouldAddToRecent) {
    zi::guard g(mutex_);

    updateSelection(segIDToOrders, shouldAddToRecent);
    OmCacheManager::TouchFreshness();
  }

  void UpdateSegmentSelection(const common::SegIDSet& ids,
                              const bool addToRecentList) {
    zi::guard g(mutex_);
    selectedIDsToOrders_.clear();

    for (auto id : ids) {
      setSegmentSelectedBatch(id, true, addToRecentList);
    }

    OmCacheManager::TouchFreshness();
  }

  void AddToSegmentSelection(const common::SegIDMap& ids) {
    zi::guard g(mutex_);
    for (auto id : ids) {
      setSegmentSelectedBatch(id.first, true, true);
    }

    OmCacheManager::TouchFreshness();
  }

  void RemoveFromSegmentSelection(const common::SegIDMap& ids) {
    zi::guard g(mutex_);
    for (auto id : ids) {
      setSegmentSelectedBatch(id.first, false, false);
    }

    OmCacheManager::TouchFreshness();
  }

  void ToggleSegmentSelection(const om::common::SegIDSet& ids) {
    bool toggle = false;
    for (auto& id : ids) {
      if (!IsSegmentSelected(id)) {
        toggle = true;
        break;
      }
    }
    for (auto& id : ids) {
      setSegmentSelectedBatch(id, toggle, true);
    }

    OmCacheManager::TouchFreshness();
  }

  void RerootSegmentList() {
    zi::guard g(mutex_);
    common::SegIDMap old = selectedIDsToOrders_;
    updateSelection(old, false);
  }

  uint32_t GetNextOrder() {
      return selectedIDsToOrders_.size() + 1;
  }

  inline void Clear() { selectedIDsToOrders_.clear(); }

  inline uint32_t GetOrderOfAdding(const common::SegID segID) {
    return selectedIDsToOrders_[segID];
  }

 private:
  zi::mutex mutex_;
  Graph& graph_;
  Store& store_;
  OmSegmentLists& lists_;

  // SegId to insertion order
  common::SegIDMap selectedIDsToOrders_;

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
    addSegmentAnyNextOrder(segID);
    if (addToRecentList) {
      addToRecentMap(segID);
    }
  }

  inline void doSelectedSetRemove(const common::SegID segID) {
    selectedIDsToOrders_.erase(segID);
    addToRecentMap(segID);
  }

  inline void addToRecentMap(const common::SegID segID) {
    OmSegment* seg = store_.GetSegment(segID);
    lists_.TouchRecent(seg);
  }

  inline void addSegmentNextOrder(const common::SegID segID) {
    uint32_t newOrder = GetNextOrder();
    addSegmentWithOrder(segID, newOrder);
  }

  inline void addSegmentWithOrder(const common::SegID segID, uint32_t newOrder) {
    selectedIDsToOrders_.insert(std::pair<common::SegID, uint32_t>(segID, newOrder));
  }

  void updateSelection(const common::SegIDMap segIDToOrders,
                       const bool shouldAddToRecent) {
    selectedIDsToOrders_.clear();
    // invert the map to get correct iteration order
    for (auto orderToSegID : orderToSegIDs(segIDToOrders)) {
      // reinsert and get's rid of missing holes in ids
      addSegmentNextOrder(orderToSegID.second);
      if (shouldAddToRecent) {
        addToRecentMap(orderToSegID.second);
      }
    }
  }

  std::map<uint32_t, common::SegID> orderToSegIDs(common::SegIDMap segIDToOrders) {
    std::map<uint32_t, common::SegID> orderToSegIDs;
    for (auto segIDToOrder : segIDToOrders) {
        orderToSegIDs[segIDToOrder.second] = segIDToOrder.first;
    }
    return orderToSegIDs;
  }

  friend class YAML::convert<OmSegmentsImpl>;
};
}
}

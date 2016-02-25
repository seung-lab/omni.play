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
  }

  bool UpdateSegmentSelection(const common::SegIDMap& segIDsToOrders,
                              const bool shouldAddToRecent) {
    zi::guard g(mutex_);
    if (segIDsToOrders == selectedIDsToOrders_) {
      return false;
    }

    updateSelection(segIDsToOrders, shouldAddToRecent);
    return true;
  }

  void UpdateSegmentSelection(const common::SegIDSet& ids,
                              const bool addToRecentList) {
    zi::guard g(mutex_);
    selectedIDsToOrders_.clear();

    for (auto id : ids) {
      setSegmentSelectedBatch(id, true, addToRecentList);
    }

  }

  void RemoveFromSegmentSelection(const common::SegIDMap& ids) {
    zi::guard g(mutex_);
    for (auto id : ids) {
      setSegmentSelectedBatch(id.first, false, false);
    }

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
    common::SegIDMap::const_iterator iter = selectedIDsToOrders_.find(segID);

    if (iter != selectedIDsToOrders_.end()) {
      return iter->second;
    } else {
      return 0;
    }
  }

  // search for the next segment after the order. if it doesn't exist return a nullptr
  std::unique_ptr<common::SegID> GetNextSegment(
      om::common::SegIDMap segIDsToOrders, uint32_t orderFrom) {
    std::map<uint32_t, common::SegID> orderToSegIDs = getOrderToSegIDs(segIDsToOrders);
    for (auto orderToSegID : orderToSegIDs) {
      if (orderToSegID.first > orderFrom) {
        return std::make_unique<uint32_t>(orderToSegID.second);
      }
    }
    return nullptr;
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
      doSelectedInsert(rootID, addToRecentList);
    } else {
      doSelectedSetRemove(rootID);
    }
  }

  inline void doSelectedInsert(const common::SegID segID,
                                  const bool addToRecentList) {
    addSegmentNextOrder(segID);
    if (addToRecentList) {
      addToRecentMap(segID);
    }
  }

  inline void doSelectedSetRemove(const common::SegID segID) {
    selectedIDsToOrders_.erase(segID);
    addToRecentMap(segID);
  }

  inline void addToRecentMap(const common::SegID segID) {
    const common::SegID rootID = graph_.Root(segID);
    OmSegment* seg = store_.GetSegment(rootID);
    lists_.TouchRecent(seg);
  }

  inline void addSegmentNextOrder(const common::SegID segID) {
    uint32_t newOrder = GetNextOrder();
    addSegmentWithOrder(segID, newOrder);
  }

  inline void addSegmentWithOrder(const common::SegID segID, uint32_t newOrder) {
    const common::SegID rootID = graph_.Root(segID);
    selectedIDsToOrders_.insert(std::pair<common::SegID, uint32_t>(rootID, newOrder));
  }

  void updateSelection(const common::SegIDMap newSelectedIDsToOrders,
                       const bool shouldAddToRecent) {
    selectedIDsToOrders_.clear();
    // invert the map to get correct iteration order
    for (auto orderToSegID : getOrderToSegIDs(newSelectedIDsToOrders)) {
      // reinsert and get's rid of missing holes in ids
      addSegmentNextOrder(orderToSegID.second);
      if (shouldAddToRecent) {
        addToRecentMap(orderToSegID.second);
      }
    }
  }

  std::map<uint32_t, common::SegID> getOrderToSegIDs(common::SegIDMap segIDsToOrders) {
    std::map<uint32_t, common::SegID> orderToSegIDs;
    for (auto segIDToOrder : segIDsToOrders) {
      orderToSegIDs.insert(std::pair<uint32_t, common::SegID>(segIDToOrder.second,
           segIDToOrder.first));
    }
    return orderToSegIDs;
  }

  friend class YAML::convert<OmSegmentsImpl>;
}; // class Selection

} //namespace segment
} //namespace om

#pragma once
#include "precomp.h"

#include "segment/lists/omSegmentLists.h"
#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "volume/omSegmentation.h"
#include "zi/omUtility.h"
#include "datalayer/archive/segmentation.h"

class OmSegmentsImpl;

class OmSegmentSelection {
 private:
  OmSegmentsImplLowLevel* const cache_;

  om::common::SegIDSet selected_;

  friend YAMLold::Emitter& YAMLold::operator<<(YAMLold::Emitter&,
                                               const OmSegmentsImpl&);
  friend void YAMLold::operator>>(const YAMLold::Node&, OmSegmentsImpl&);
  friend QDataStream& operator<<(QDataStream&, const OmSegmentsImpl&);
  friend QDataStream& operator>>(QDataStream&, OmSegmentsImpl&);

  inline void addToRecentMap(const om::common::SegID segID) {
    OmSegment* seg = cache_->SegmentStore()->GetSegment(segID);
    cache_->segmentation_->SegmentLists()->TouchRecent(seg);
  }

 public:
  OmSegmentSelection(OmSegmentsImplLowLevel* cache) : cache_(cache) {}

  inline const om::common::SegIDSet GetSelectedSegmentIDs() const {
    return selected_;
  }

  inline uint32_t NumberOfSelectedSegments() const { return selected_.size(); }

  inline void Clear() { selected_.clear(); }

  void rerootSegmentList() {
    om::common::SegIDSet old = selected_;
    selected_.clear();

    for (auto& id : old) {
      selected_.insert(cache_->FindRootID(id));
    }
  }

  inline bool AreSegmentsSelected() const { return !selected_.empty(); }

  inline bool isSegmentSelected(const om::common::SegID segID) const {
    if (selected_.empty()) {
      return false;
    }
    return selected_.count(cache_->FindRootID(segID));
  }

  inline void setSegmentSelected(om::common::SegID segID, const bool isSelected,
                                 const bool addToRecentList) {
    setSegmentSelectedBatch(segID, isSelected, addToRecentList);
    cache_->touchFreshness();
  }

  void UpdateSegmentSelection(const om::common::SegIDSet& ids,
                              const bool addToRecentList) {
    selected_.clear();

    for (auto& id : ids) {
      setSegmentSelectedBatch(id, true, addToRecentList);
    }

    cache_->touchFreshness();
  }

  void AddToSegmentSelection(const om::common::SegIDSet& ids) {
    for (auto& id : ids) {
      setSegmentSelectedBatch(id, true, true);
    }

    cache_->touchFreshness();
  }

  void ToggleSegmentSelection(const om::common::SegIDSet& ids) {
    bool toggle = false;
    for (auto& id : ids) {
      if (!isSegmentSelected(id)) {
        toggle = true;
        break;
      }
    }
    for (auto& id : ids) {
      setSegmentSelectedBatch(id, toggle, true);
    }

    cache_->touchFreshness();
  }

  void RemoveFromSegmentSelection(const om::common::SegIDSet& ids) {
    for (auto& id : ids) {
      setSegmentSelectedBatch(id, false, false);
    }

    cache_->touchFreshness();
  }

  inline void setSegmentSelectedBatch(const om::common::SegID segID,
                                      const bool isSelected,
                                      const bool addToRecentList) {
    const om::common::SegID rootID = cache_->FindRootID(segID);

    if (isSelected) {
      doSelectedSetInsert(rootID, addToRecentList);
    } else {
      doSelectedSetRemove(rootID);
    }
  }

  inline void doSelectedSetInsert(const om::common::SegID segID,
                                  const bool addToRecentList) {
    selected_.insert(segID);
    if (addToRecentList) {
      addToRecentMap(segID);
    }
  }

  inline void doSelectedSetRemove(const om::common::SegID segID) {
    selected_.erase(segID);
    addToRecentMap(segID);
  }
};

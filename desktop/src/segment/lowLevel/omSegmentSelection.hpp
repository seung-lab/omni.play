#pragma once

#include "segment/lists/omSegmentLists.h"
#include "segment/lowLevel/omSegmentsImplLowLevel.h"
#include "volume/omSegmentation.h"
#include "zi/omUtility.h"
#include "datalayer/archive/segmentation.h"

class OmSegmentsImpl;

class OmSegmentSelection {
 private:
  OmSegmentsImplLowLevel* const cache_;

  OmSegIDsSet selected_;

  friend YAML::Emitter& YAML::operator<<(YAML::Emitter&, const OmSegmentsImpl&);
  friend void YAML::operator>>(const YAML::Node&, OmSegmentsImpl&);
  friend QDataStream& operator<<(QDataStream&, const OmSegmentsImpl&);
  friend QDataStream& operator>>(QDataStream&, OmSegmentsImpl&);

  inline void addToRecentMap(const OmSegID segID) {
    OmSegment* seg = cache_->SegmentStore()->GetSegment(segID);
    cache_->segmentation_->SegmentLists()->TouchRecent(seg);
  }

 public:
  OmSegmentSelection(OmSegmentsImplLowLevel* cache) : cache_(cache) {}

  inline const OmSegIDsSet GetSelectedSegmentIDs() const { return selected_; }

  inline uint32_t NumberOfSelectedSegments() const { return selected_.size(); }

  inline void Clear() { selected_.clear(); }

  void rerootSegmentList() {
    OmSegIDsSet old = selected_;
    selected_.clear();

    FOR_EACH(iter, old) { selected_.insert(cache_->FindRootID(*iter)); }
  }

  inline bool AreSegmentsSelected() const { return !selected_.empty(); }

  inline bool isSegmentSelected(const OmSegID segID) const {
    if (selected_.empty()) {
      return false;
    }
    return selected_.count(cache_->FindRootID(segID));
  }

  inline void setSegmentSelected(OmSegID segID, const bool isSelected,
                                 const bool addToRecentList) {
    setSegmentSelectedBatch(segID, isSelected, addToRecentList);
    cache_->touchFreshness();
  }

  void UpdateSegmentSelection(const OmSegIDsSet& ids,
                              const bool addToRecentList) {
    selected_.clear();

    FOR_EACH(iter, ids) {
      setSegmentSelectedBatch(*iter, true, addToRecentList);
    }

    cache_->touchFreshness();
  }

  void AddToSegmentSelection(const OmSegIDsSet& ids) {
    FOR_EACH(iter, ids) { setSegmentSelectedBatch(*iter, true, true); }

    cache_->touchFreshness();
  }

  void RemoveFromSegmentSelection(const OmSegIDsSet& ids) {
    FOR_EACH(iter, ids) { setSegmentSelectedBatch(*iter, false, false); }

    cache_->touchFreshness();
  }

  inline void setSegmentSelectedBatch(const OmSegID segID,
                                      const bool isSelected,
                                      const bool addToRecentList) {
    const OmSegID rootID = cache_->FindRootID(segID);

    if (isSelected) {
      doSelectedSetInsert(rootID, addToRecentList);
    } else {
      doSelectedSetRemove(rootID);
    }
  }

  inline void doSelectedSetInsert(const OmSegID segID,
                                  const bool addToRecentList) {
    selected_.insert(segID);
    if (addToRecentList) {
      addToRecentMap(segID);
    }
  }

  inline void doSelectedSetRemove(const OmSegID segID) {
    selected_.erase(segID);
    addToRecentMap(segID);
  }
};

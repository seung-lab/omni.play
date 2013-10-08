#pragma once

#include "common/omCommon.h"
#include "segment/omSegment.h"
#include "segment/omSegmentChildrenTypes.h"

class OmSegmentChildren {
 private:
  std::vector<segChildCont_t> list_;

 public:
  OmSegmentChildren(const size_t size) { list_.resize(size); }

  inline void Resize(const size_t size) { list_.resize(size); }

  inline const segChildCont_t& GetChildren(const OmSegID segID) const {
    return list_[segID];
  }

  inline const segChildCont_t& GetChildren(OmSegment* seg) const {
    return list_[seg->value()];
  }

  inline void AddChild(const OmSegID segID, OmSegment* child) {
    list_[segID].insert(child);
  }

  inline void AddChild(OmSegment* seg, OmSegment* child) {
    list_[seg->value()].insert(child);
  }

  inline void RemoveChild(const OmSegID segID, OmSegment* child) {
    list_[segID].erase(child);
  }

  inline void RemoveChild(OmSegment* seg, OmSegment* child) {
    list_[seg->value()].erase(child);
  }
};

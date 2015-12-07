#pragma once
#include "precomp.h"

#include "segment/omSegment.h"
#include "segment/omSegments.h"

class OmProcessSegmentationChunk {
 public:
  OmProcessSegmentationChunk(const bool computeSizes, const bool computeBounds, OmSegments& segments)
      : computeSizes_(computeSizes), computeBounds_(computeBounds), segments_(segments) {}

  ~OmProcessSegmentationChunk() {
    for (auto& iter : cacheSegments_) {
      const om::common::SegID val = iter.first;
      OmSegment* seg = iter.second;
      if (computeSizes_) {
        seg->addToSize(sizes_[val]);
      }
      if (computeBounds_) {
        seg->AddToBoundingBox(bounds_.at(val));
      }
    }
  }

  inline void processVoxel(const om::common::SegID val,
                           const om::coords::Data& dc) {
    getOrAddSegment(val);

    if (computeBounds_) {
      auto iter = bounds_.find(val);
      if (iter == bounds_.end()) {
        bounds_.emplace(val, om::coords::DataBbox(dc, dc));
      } else {
        iter->second.merge(om::coords::DataBbox(dc, dc));
      }
    }

    if (computeSizes_) {
      sizes_[val] = 1 + sizes_[val];
    }
  }

 private:
  const bool computeSizes_;
  const bool computeBounds_;
  OmSegments& segments_;

  std::unordered_map<om::common::SegID, OmSegment*> cacheSegments_;
  std::unordered_map<om::common::SegID, uint64_t> sizes_;
  typedef std::unordered_map<om::common::SegID, om::coords::DataBbox> bbox_map;
  bbox_map bounds_;

  OmSegment* getOrAddSegment(const om::common::SegID val) {
    if (!cacheSegments_.count(val)) {
      sizes_[val] = 0;
      return cacheSegments_[val] = segments_.GetOrAddSegment(val);
    }
    return cacheSegments_[val];
  }
};

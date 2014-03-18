#pragma once
#include "precomp.h"

#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "chunks/omChunk.h"

class OmProcessSegmentationChunk {
 public:
  OmProcessSegmentationChunk(OmChunk* chunk, const bool computeSizes,
                             OmSegments* segments)
      : chunk_(chunk),
        computeSizes_(computeSizes),
        minVertexOfChunk_(chunk_->Mipping().Extent().getMin()),
        segments_(segments) {}

  ~OmProcessSegmentationChunk() {
    FOR_EACH(iter, cacheSegments_) {
      const om::common::SegID val = iter->first;
      OmSegment* seg = iter->second;
      seg->addToSize(sizes_[val]);
      seg->AddToBoundingBox(
          om::coords::DataBbox(bounds_[val], minVertexOfChunk_.volume(), 0));
    }
  }

  inline void processVoxel(const om::common::SegID val,
                           const Vector3i& voxelPos) {
    if (!computeSizes_) {
      return;
    }

    getOrAddSegment(val);
    sizes_[val] = 1 + sizes_[val];
    bounds_[val].merge(om::coords::DataBbox(minVertexOfChunk_ + voxelPos,
                                            minVertexOfChunk_ + voxelPos));
  }

 private:
  OmChunk* const chunk_;
  const bool computeSizes_;
  const om::coords::Data minVertexOfChunk_;
  OmSegments* const segments_;

  std::unordered_map<om::common::SegID, OmSegment*> cacheSegments_;
  std::unordered_map<om::common::SegID, uint64_t> sizes_;
  typedef std::unordered_map<om::common::SegID, AxisAlignedBoundingBox<int> >
      bbox_map;
  bbox_map bounds_;

  OmSegment* getOrAddSegment(const om::common::SegID val) {
    if (!cacheSegments_.count(val)) {
      return cacheSegments_[val] = segments_->GetOrAddSegment(val);
    }
    return cacheSegments_[val];
  }
};

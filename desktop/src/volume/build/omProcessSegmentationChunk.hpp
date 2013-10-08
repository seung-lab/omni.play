#pragma once

#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "chunks/omChunk.h"

class OmProcessSegmentationChunk {
 public:
  OmProcessSegmentationChunk(OmChunk* chunk, const bool computeSizes,
                             OmSegments* segments)
      : chunk_(chunk),
        computeSizes_(computeSizes),
        minVertexOfChunk_(chunk_->Mipping().GetExtent().getMin()),
        segments_(segments) {}

  ~OmProcessSegmentationChunk() {
    FOR_EACH(iter, cacheSegments_) {
      const OmSegID val = iter->first;
      OmSegment* seg = iter->second;
      seg->addToSize(sizes_[val]);
      seg->AddToBoundingBox(
          om::dataBbox(bounds_[val], minVertexOfChunk_.volume(), 0));
    }
  }

  inline void processVoxel(const OmSegID val, const Vector3i& voxelPos) {
    if (!computeSizes_) {
      return;
    }

    getOrAddSegment(val);
    sizes_[val] = 1 + sizes_[val];
    bounds_[val].merge(om::dataBbox(minVertexOfChunk_ + voxelPos,
                                    minVertexOfChunk_ + voxelPos));
  }

 private:
  OmChunk* const chunk_;
  const bool computeSizes_;
  const om::dataCoord minVertexOfChunk_;
  OmSegments* const segments_;

  boost::unordered_map<OmSegID, OmSegment*> cacheSegments_;
  boost::unordered_map<OmSegID, uint64_t> sizes_;
  typedef boost::unordered_map<OmSegID, AxisAlignedBoundingBox<int> > bbox_map;
  bbox_map bounds_;

  OmSegment* getOrAddSegment(const OmSegID val) {
    if (!cacheSegments_.count(val)) {
      return cacheSegments_[val] = segments_->GetOrAddSegment(val);
    }
    return cacheSegments_[val];
  }
};

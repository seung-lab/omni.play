#pragma once

#include "segment/omSegments.h"

namespace om {
namespace segchunk {

class ProcessChunkVoxelBoundingData {
 public:
  ProcessChunkVoxelBoundingData(OmSegChunk* chunk, OmSegments* segments)
      : chunk_(chunk),
        minVertexOfChunk_(chunk_->Mipping().GetExtent().getMin()),
        segments_(segments) {}

  ~ProcessChunkVoxelBoundingData() {
    FOR_EACH(iter, cacheSegments_) {
      const OmSegID val = iter->first;
      OmSegment* seg = iter->second;
      seg->AddToBoundingBox(getBbox(val).get());
    }
  }

  inline void processVoxel(const OmSegID val, const Vector3i& voxelPos) {
    getOrAddSegment(val);
    getBbox(val).get().merge(om::dataBbox(minVertexOfChunk_ + voxelPos,
                                          minVertexOfChunk_ + voxelPos));
  }

 private:
  OmSegChunk* const chunk_;
  const om::dataCoord minVertexOfChunk_;
  OmSegments* const segments_;

  boost::unordered_map<OmSegID, OmSegment*> cacheSegments_;
  typedef boost::unordered_map<OmSegID, om::dataBbox> bbox_map;
  bbox_map bounds_;

  OmSegment* getOrAddSegment(const OmSegID val) {
    if (!cacheSegments_.count(val)) {
      return cacheSegments_[val] = segments_->GetOrAddSegment(val);
    }
    return cacheSegments_[val];
  }

  boost::optional<om::dataBbox&> getBbox(OmSegID id) {
    bbox_map::iterator bbox = bounds_.find(id);
    if (bbox != bounds_.end()) {
      return bbox->second;
    } else {
      return false;
    }
  }
};

}  // namespace segchunk
}  // namespace om

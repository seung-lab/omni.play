#pragma once

#include "segment/omSegment.h"
#include "segment/omSegments.h"

namespace om {
namespace segchunk {

class ProcessChunkVoxelBoundingData {
 public:
  ProcessChunkVoxelBoundingData(OmSegChunk* chunk, OmSegments* segments)
      : chunk_(chunk),
        minVertexOfChunk_(chunk_->Mipping().Extent().getMin()),
        segments_(segments) {}

  ~ProcessChunkVoxelBoundingData() {
    FOR_EACH(iter, cacheSegments_) {
      const om::common::SegID val = iter->first;
      OmSegment* seg = iter->second;
      seg->AddToBoundingBox(getBbox(val).get());
    }
  }

  inline void processVoxel(const om::common::SegID val,
                           const Vector3i& voxelPos) {
    getOrAddSegment(val);
    getBbox(val).get().merge(om::coords::DataBbox(
        minVertexOfChunk_ + voxelPos, minVertexOfChunk_ + voxelPos));
  }

 private:
  OmSegChunk* const chunk_;
  const om::coords::Data minVertexOfChunk_;
  OmSegments* const segments_;

  std::unordered_map<om::common::SegID, OmSegment*> cacheSegments_;
  typedef std::unordered_map<om::common::SegID, om::coords::DataBbox> bbox_map;
  bbox_map bounds_;

  OmSegment* getOrAddSegment(const om::common::SegID val) {
    if (!cacheSegments_.count(val)) {
      return cacheSegments_[val] = segments_->GetOrAddSegment(val);
    }
    return cacheSegments_[val];
  }

  boost::optional<om::coords::DataBbox&> getBbox(om::common::SegID id) {
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

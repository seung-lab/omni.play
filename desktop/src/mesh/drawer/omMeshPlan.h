#pragma once

#include <map>
#include "coordinates/chunkCoord.h"
#include "segment/omSegment.h"

struct LargestSegmentFirst {
  bool operator()(const OmSegment* a, const OmSegment* b) const {
    return a->size() > b->size();
  }
};

typedef std::multimap<OmSegment*, om::chunkCoord, LargestSegmentFirst>
    OmMeshPlanStruct;

class OmMeshPlan : public OmMeshPlanStruct {
 private:
  uint64_t voxelCount_;

 public:
  OmMeshPlan() : voxelCount_(0) {}

  void Add(OmSegment* seg, const om::chunkCoord& coord) {
    insert(std::make_pair(seg, coord));
    voxelCount_ += seg->size();
  }

  uint64_t TotalVoxels() const { return voxelCount_; }
};

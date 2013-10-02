#pragma once

#include "segment/omSegment.h"

namespace om {
namespace v3d {

struct MeshPlanKey {
  OmSegment* seg;
  float distance;

  // largest segments, closest to camera, first
  bool operator<(const MeshPlanKey& a) const {
    if (qFuzzyCompare(distance, a.distance)) {
      return seg->size() > a.seg->size();
    }
    return distance < a.distance;
  }
};

struct MeshPlanPayload {
  om::coords::Chunk coord;
  Vector3f color;
};

typedef std::multimap<MeshPlanKey, MeshPlanPayload> MeshPlanStruct;

class MeshPlan : public MeshPlanStruct {
 private:
  bool allSegmentsFound_;

 public:
  MeshPlan() : allSegmentsFound_(true) {}

  void Add(OmSegment* seg, const om::coords::Chunk& coord, float distance,
           const Vector3f& color) {
    insert(std::make_pair(MeshPlanKey({seg, distance}),
                          MeshPlanPayload({coord, color})));
  }

  bool HasAllSegments() const { return allSegmentsFound_; }

  void SetSegmentsAsMissing() { allSegmentsFound_ = false; }
};
}
}  // om::v3d::

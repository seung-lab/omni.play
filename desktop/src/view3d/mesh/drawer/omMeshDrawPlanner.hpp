#pragma once

#include "viewGroup/omViewGroupState.h"
#include "view3d/mesh/drawer/omFindChunksToDraw.hpp"
#include "view3d/mesh/drawer/omMeshPlan.h"
#include "view3d/mesh/drawer/omMeshSegmentList.hpp"
#include "volume/omSegmentation.h"
#include "segment/selection.hpp"

namespace om {
namespace v3d {

class MeshDrawPlanner {
 private:
  OmSegments& segments_;
  MeshSegmentList& rootSegList_;
  FindChunksToDraw findChunks_;

  drawChunks_t drawChunks_;

 public:
  MeshDrawPlanner(const OmMipVolCoords& system, OmSegments& segments,
                  MeshSegmentList& rootSegList)
      : segments_(segments),
        rootSegList_(rootSegList),
        findChunks_(system, drawChunks_) {}

  void Reset() { drawChunks_.clear(); }

  std::shared_ptr<MeshPlan> BuildPlan(const OmVolumeCuller& culler,
                                      const key& key) {
    auto plan = std::make_shared<MeshPlan>();

    auto rootSegs = segments_.Selection().GetSelectedSegmentIDs();
    if (!rootSegs.size()) {
      return plan;
    }

    // might be called extraneously if volume is not in camera....
    if (drawChunks_.empty()) {
      findChunks_.Find(culler);
    }

    for (auto& cd : drawChunks_) {
      for (auto& segID : rootSegs) {
        auto segmentsToDraw = rootSegList_.Get(cd.coord, segID, key);

        if (!segmentsToDraw) {
          plan->SetSegmentsAsMissing();
          continue;
        }

        for (auto& s : *segmentsToDraw) {
          plan->Add(s.seg, cd.coord, cd.distance, s.color);
        }
      }
    }

    return plan;
  }
};
}
}  // om::v3d::

#pragma once

#include "mesh/drawer/struct.hpp"
#include "mesh/drawer/omMeshPlan.h"
#include "mesh/drawer/omMeshDrawPlanner.hpp"
#include "system/cache/omCacheManager.h"

namespace om {
namespace v3d {

class PlanCache {
 private:
  OmSegmentation& vol_;
  OmMeshSegmentList& rootSegLists_;
  OmViewGroupState& vgs_;
  MeshDrawPlanner planner_;

  // depends upon culler
  std::map<key, std::shared_ptr<MeshPlan>> meshPlans_;

 public:
  PlanCache(OmSegmentation& vol, OmMeshSegmentList& rootSegLists,
            OmViewGroupState& vgs)
      : vol_(vol),
        rootSegLists_(rootSegLists),
        vgs_(vgs),
        planner_(vol_.Coords(), vol_.Segments(), rootSegLists_) {}

  std::shared_ptr<MeshPlan> GetPlan(const OmVolumeCuller& culler,
                                    const bool changed) {
    key k = {OmCacheManager::GetFreshness(),   vgs_.getDustThreshold(),
             vgs_.shouldVolumeBeShownBroken(), vgs_.getBreakThreshold()};

    if (changed) {
      planner_.Reset();
      meshPlans_.clear();
      return buildPlan(culler, k);
    }

    auto iter = meshPlans_.find(k);
    if (iter == meshPlans_.end()) {
      return buildPlan(culler, k);
    }
    return iter->second;
  }

 private:
  std::shared_ptr<MeshPlan> buildPlan(const OmVolumeCuller& culler,
                                      const key& k) {
    auto meshPlan = planner_.BuildPlan(culler, k);

    if (meshPlan->HasAllSegments()) {
      meshPlans_[k] = meshPlan;  // cache full plan
    }

    return meshPlan;
  }
};
}
}  // om::v3d::

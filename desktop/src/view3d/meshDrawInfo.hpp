#pragma once
#include "precomp.h"

#include "mesh/drawer/omMeshDrawerImpl.hpp"
#include "mesh/drawer/omMeshPlanCache.hpp"

namespace om {
namespace v3d {

class MeshDrawInfo {
 private:
  OmSegmentation& vol_;
  MeshSegmentList rootSegLists_;
  PlanCache cache_;
  DrawerImpl drawer_;

  std::shared_ptr<OmVolumeCuller> culler_;
  int numPrevRedraws_;

 public:
  MeshDrawInfo(OmSegmentation& vol, OmViewGroupState& vgs)
      : vol_(vol),
        rootSegLists_(vol),
        cache_(vol.Coords(), vol.Segments(), rootSegLists_, vgs),
        drawer_(vol.Coords(), vol.id()),
        numPrevRedraws_(0) {}

  inline OmSegmentation& Vol() const { return vol_; }

  inline std::shared_ptr<MeshPlan> GetPlan(const OmVolumeCuller& culler,
                                           const bool changed) {
    return cache_.GetPlan(culler, changed);
  }

  inline DrawerImpl& Drawer() { return drawer_; }

  inline bool DidCullerChange(std::shared_ptr<OmVolumeCuller> culler) {
    if (!culler_ || *culler_ != *culler) {
      culler_ = culler;
      numPrevRedraws_ = 0;
      return true;
    }
    ++numPrevRedraws_;
    return false;
  }

  inline int GetAllowedDrawTime() {
    static const int allowedMS = 50;  // attempt 20 fps...
    static const int maxMS = 250;
    static const int numRoundsBeforeUpMaxTime = 10;

    if (numPrevRedraws_ < numRoundsBeforeUpMaxTime) {
      return allowedMS;
    }

    // TODO: shouldn't there be a cast to float?
    const int maxRedrawMS =
        (numPrevRedraws_ / numRoundsBeforeUpMaxTime) * allowedMS;

    if (maxRedrawMS > maxMS) {
      return maxMS;
    }

    return maxRedrawMS;
  }
};
}
}  // om::v3d::

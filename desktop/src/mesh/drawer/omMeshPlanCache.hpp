#pragma once
#include "precomp.h"

#include "mesh/drawer/omMeshPlan.h"
#include "mesh/drawer/omMeshDrawPlanner.hpp"
#include "system/cache/omCacheManager.h"

class OmMeshPlanCache {
 private:
  OmSegmentation* const segmentation_;
  OmMeshSegmentList* const rootSegLists_;

  struct CachedDataEnry {
    std::shared_ptr<OmMeshPlan> sortedSegments;
    uint64_t freshness;
    uint64_t dustThreshold;
  };

  struct CachedData {
    std::shared_ptr<OmVolumeCuller> culler;
    std::shared_ptr<std::deque<OmSegChunk*> > chunks;
    std::map<OmBitfield, CachedDataEnry> dataByBitfield;
  };

  CachedData cachedData_;

 public:
  OmMeshPlanCache(OmSegmentation* segmentation, OmMeshSegmentList* rootSegLists)
      : segmentation_(segmentation), rootSegLists_(rootSegLists) {}

  std::shared_ptr<OmMeshPlan> GetSegmentsToDraw(
      OmViewGroupState& vgs, std::shared_ptr<OmVolumeCuller> culler,
      const OmBitfield drawOptions) {
    std::shared_ptr<OmMeshPlan> sortedSegments =
        getCachedSegments(vgs, culler, drawOptions);

    if (sortedSegments) {
      return sortedSegments;
    }

    return buildPlan(vgs, culler, drawOptions);
  }

 private:
  /** cached segment list should be the same if
   *  1.) cullers are the same
   *  2.) same segments are selected (i.e. same global freshness)
   *  3.) same View3d dusting threshold
   *
   *  a list gets cached ONLY once all segments have been added to the list
   **/
  std::shared_ptr<OmMeshPlan> getCachedSegments(
      OmViewGroupState& vgs, std::shared_ptr<OmVolumeCuller> culler,
      const OmBitfield drawOptions) {
    if (!cachedData_.culler) {
      return std::shared_ptr<OmMeshPlan>();
    }

    if (!cachedData_.culler->equals(culler)) {
      cachedData_.dataByBitfield.clear();
      cachedData_.chunks.reset();
      return std::shared_ptr<OmMeshPlan>();
    }

    if (!cachedData_.dataByBitfield.count(drawOptions)) {
      return std::shared_ptr<OmMeshPlan>();
    }

    const CachedDataEnry& entry = cachedData_.dataByBitfield[drawOptions];

    if (entry.freshness == OmCacheManager::GetFreshness() &&
        entry.dustThreshold == vgs->getDustThreshold()) {
      return entry.sortedSegments;
    }

    return std::shared_ptr<OmMeshPlan>();
  }

  std::shared_ptr<OmMeshPlan> buildPlan(OmViewGroupState& vgs,
                                        std::shared_ptr<OmVolumeCuller> culler,
                                        const OmBitfield drawOptions) {
    OmMeshDrawPlanner planner(segmentation_, vgs, culler, drawOptions,
                              rootSegLists_);

    std::shared_ptr<OmMeshPlan> sortedSegments =
        planner.BuildPlan(cachedData_.chunks);

    if (!planner.SegmentsMissing()) {
      CachedDataEnry data;
      data.sortedSegments = sortedSegments;
      data.freshness = OmCacheManager::GetFreshness();
      data.dustThreshold = vgs->getDustThreshold();

      cachedData_.dataByBitfield[drawOptions] = data;
    }

    cachedData_.culler = culler;
    cachedData_.chunks = planner.GetChunkList();

    return sortedSegments;
  }
};

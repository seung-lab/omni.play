#pragma once

#include "view3d/mesh/drawer/struct.hpp"
#include "common/common.h"
#include "view3d/mesh/drawer/omMeshSegmentListTask.h"
#include "view3d/mesh/drawer/omMeshSegmentListTypes.hpp"
#include "threads/taskManager.hpp"
#include "segment/omSegment.h"
#include "volume/omSegmentation.h"
#include <map>

namespace om {
namespace v3d {

class MeshSegmentList {
 private:
  static const int MAX_THREADS = 3;
  OmSegmentation& vol_;

 public:
  MeshSegmentList(OmSegmentation& vol) : vol_(vol) {
    threadPool_.start(MAX_THREADS);
  }

  ~MeshSegmentList() {
    threadPool_.clear();
    threadPool_.stop();
  }

  boost::optional<SegPtrAndColorList> Get(const om::coords::Chunk& coord,
                                          om::common::SegID segID,
                                          const om::v3d::key& key) {
    zi::guard g(lock_);

    SegPtrAndColorListValid& spList = mSegmentListCache[makeKey(
        coord, segID, key.shouldVolumeBeShownBroken, key.breakThreshold)];

    if (spList.isFetching) {  // coord already in queue to be fetched
      return boost::optional<SegPtrAndColorList>();
    }

    // remove from cache if freshness is too old
    auto* rootSeg = vol_.Segments()->GetSegment(segID);
    if (!rootSeg) {
      return boost::optional<SegPtrAndColorList>();
    }

    const uint32_t currentFreshness = rootSeg->getFreshnessForMeshes();
    if (spList.isValid && currentFreshness != spList.freshness) {
      spList.list.clear();
      spList.isValid = false;
    }

    if (!spList.isValid) {  // add coord to list to be fetched
      spList = SegPtrAndColorListValid(true);

      auto task = std::make_shared<MeshSegmentListTask>(
          *this, vol_.UniqueValuesDS(), *vol_.Segments(), coord, *rootSeg, key);

      threadPool_.push_back(task);
      return boost::optional<SegPtrAndColorList>();
    }

    // coord was valid
    return boost::optional<SegPtrAndColorList>(spList.list);
  }

  void AddToCache(const om::coords::Chunk& coord, OmSegment& rootSeg,
                  bool shouldVolumeBeShownBroken, float breakThreshold,
                  const SegPtrAndColorList& segmentsToDraw) {
    zi::guard g(lock_);

    mSegmentListCache[makeKey(coord, rootSeg.value(), shouldVolumeBeShownBroken,
                              breakThreshold)] =
        SegPtrAndColorListValid(segmentsToDraw,
                                rootSeg.getFreshnessForMeshes());
  }

 private:
  std::map<MeshSegListKey, SegPtrAndColorListValid> mSegmentListCache;
  om::thread::ThreadPool threadPool_;
  zi::mutex lock_;

  MeshSegListKey makeKey(const om::coords::Chunk& coord,
                         om::common::SegID segID,
                         bool shouldVolumeBeShownBroken, float breakThreshold) {
    return MeshSegListKey(vol_.id(), segID, coord.mipLevel(), coord.x, coord.y,
                          coord.z, shouldVolumeBeShownBroken, breakThreshold);
  }
};
}
}  // namespace

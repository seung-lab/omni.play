#pragma once

#include "view2d/omView2dConverters.hpp"
#include "volume/omSegmentation.h"

class OmFillTool {
 private:
  const SegmentDataWrapper sdw_;
  const uint32_t newSegID_;
  const ViewType viewType_;
  OmSegmentation& vol_;
  const om::globalBbox segDataExtent_;
  OmSegments* const segments_;

  zi::semaphore semaphore_;

 public:
  OmFillTool(const SegmentDataWrapper& sdw, const ViewType viewType)
      : sdw_(sdw),
        newSegID_(sdw.getID()),
        viewType_(viewType),
        vol_(sdw.GetSegmentation()),
        segDataExtent_(vol_.Coords().GetExtent()),
        segments_(vol_.Segments()) {}

  ~OmFillTool() {}

  void Fill(const om::globalCoord& v) {
    if (!segDataExtent_.contains(v)) {
      return;
    }

    const OmSegID segIDtoReplace = segments_->findRootID(vol_.GetVoxelValue(v));

    vol_.SetVoxelValue(v, newSegID_);

    std::deque<om::globalCoord> voxels;

    voxels.push_back(om::globalCoord(v.x - 1, v.y, v.z));
    voxels.push_back(om::globalCoord(v.x + 1, v.y, v.z));
    voxels.push_back(om::globalCoord(v.x, v.y - 1, v.z));
    voxels.push_back(om::globalCoord(v.x, v.y + 1, v.z));

    semaphore_.set(0);

    FOR_EACH(iter, voxels) {
      OmView2dManager::AddTaskBack(zi::run_fn(
          zi::bind(&OmFillTool::doFill, this, *iter, segIDtoReplace)));
    }

    semaphore_.acquire(voxels.size());

    clearCaches();

    OmEvents::Redraw2d();
  }

 private:

  void doFill(const om::globalCoord voxelLocStart,
              const OmSegID segIDtoReplace) {
    std::deque<om::globalCoord> voxels;
    voxels.push_back(voxelLocStart);

    om::globalCoord v;

    while (!voxels.empty()) {
      v = voxels.back();
      voxels.pop_back();

      if (!segDataExtent_.contains(v)) {
        continue;
      }

      const OmSegID curSegID = vol_.GetVoxelValue(v);

      if (newSegID_ == curSegID) {
        continue;
      }

      if (segIDtoReplace != curSegID &&
          segIDtoReplace != segments_->findRootID(curSegID)) {
        continue;
      }

      vol_.SetVoxelValue(v, newSegID_);

      // TODO: assumes XY_VIEW for now
      voxels.push_back(om::globalCoord(v.x - 1, v.y, v.z));
      voxels.push_back(om::globalCoord(v.x + 1, v.y, v.z));
      voxels.push_back(om::globalCoord(v.x, v.y - 1, v.z));
      voxels.push_back(om::globalCoord(v.x, v.y + 1, v.z));
    }

    semaphore_.release(1);
  }

  void clearCaches() {
    vol_.SliceCache()->Clear();
    OmTileCache::ClearSegmentation();
  }
};

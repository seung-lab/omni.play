#pragma once

#include "view2d/omSliceCache.hpp"

class OmChunksAndPts {
 private:
  OmSegmentation* const vol_;
  const om::common::ViewType viewType_;

  std::set<om::dataCoord> pts_;

 public:
  OmChunksAndPts(OmSegmentation* vol, const om::common::ViewType viewType)
      : vol_(vol), viewType_(viewType) {}

  void AddAllPtsThatIntersectVol(om::pt3d_list_t* pts) {
    const om::globalBbox& segDataExtent = vol_->Coords().GetExtent();

    FOR_EACH(iter, *pts) {
      if (!segDataExtent.contains(*iter)) {
        continue;
      }

      om::dataCoord coord = iter->toDataCoord(vol_, 0);

      pts_.insert(coord);
    }
  }

  std::shared_ptr<om::common::SegIDSet> GetSegIDs() {
    OmSliceCache sliceCache(vol_, viewType_);

    std::shared_ptr<om::common::SegIDSet> ret =
        std::make_shared<om::common::SegIDSet>();

    FOR_EACH(pt, pts_) {
      om::common::SegID id = sliceCache.GetVoxelValue(*pt);
      if (id) {
        ret->insert(id);
      }
    }

    return ret;
  }
};

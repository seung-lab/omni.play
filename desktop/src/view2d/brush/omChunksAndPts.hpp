#pragma once

#include "view2d/omSliceCache.hpp"

class OmChunksAndPts {
 private:
  OmSegmentation* const vol_;
  const om::common::ViewType viewType_;

  std::set<om::coords::Data> pts_;

 public:
  OmChunksAndPts(OmSegmentation* vol, const om::common::ViewType viewType)
      : vol_(vol), viewType_(viewType) {}

  void AddAllPtsThatIntersectVol(om::pt3d_list_t* pts) {
    const om::coords::GlobalBbox& segDataExtent = vol_->Coords().Extent();

    FOR_EACH(iter, *pts) {
      if (!segDataExtent.contains(*iter)) {
        continue;
      }

      om::coords::Data coord = iter->ToData(vol_->Coords(), 0);

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

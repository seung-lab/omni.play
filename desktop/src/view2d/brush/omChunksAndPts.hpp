#pragma once
#include "precomp.h"

#include "chunk/voxelGetter.hpp"

class OmChunksAndPts {
 private:
  OmSegmentation& vol_;
  const om::common::ViewType viewType_;

  std::set<om::coords::Data> pts_;

 public:
  OmChunksAndPts(OmSegmentation& vol, const om::common::ViewType viewType)
      : vol_(vol), viewType_(viewType) {}

  void AddAllPtsThatIntersectVol(om::pt3d_list_t const& pts) {
    const om::coords::GlobalBbox& segDataExtent = vol_.Coords().Extent();

    FOR_EACH(iter, pts) {
      if (!segDataExtent.contains(*iter)) {
        continue;
      }

      om::coords::Data coord = iter->ToData(vol_.Coords(), 0);

      pts_.insert(coord);
    }
  }

  om::common::SegIDSet GetSegIDs() {
    om::chunk::Voxels<uint32_t> voxels(vol_.ChunkDS(), vol_.Coords());

    om::common::SegIDSet ret;

    for (auto& pt : pts_) {
      om::common::SegID id = voxels.GetValue(pt);
      if (id) {
        ret.insert(id);
      }
    }

    return ret;
  }
};

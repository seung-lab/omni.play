#pragma once
#include "precomp.h"

#include "events/events.h"
#include "actions/omActions.h"
#include "tiles/cache/omTileCache.h"
#include "view2d/brush/omBrushOppTypes.h"

#include "tiles/cache/raw/omRawSegTileCache.hpp"
#include "utility/segmentationDataWrapper.hpp"

class OmBrushPaintUtils {
 public:
  static void PaintPts(std::shared_ptr<OmBrushOppInfo> info,
                       om::pt3d_list_t* pts,
                       const om::common::SegID segIDtoPaint) {
    if (!info) {
      throw om::ArgException("invalid OmBrushOppInfo");
    }
    auto& coords = info->segmentation.Coords();
    auto segDataExtent = coords.Extent();

    std::set<om::coords::Data> voxelCoords;

    for (auto& iter : *pts) {
      if (!segDataExtent.contains(iter)) {
        continue;
      }
      auto dc = om::coords::Data(iter, coords, 0);
      voxelCoords.insert(dc);
      for (int level = 1; level <= coords.RootMipLevel(); level++) {
        voxelCoords.insert(dc.AtDifferentLevel(level));
      }
    }

    if (!voxelCoords.size()) {
      return;
    }

    OmActions::SetVoxels(info->segmentation.GetID(), voxelCoords, segIDtoPaint);

    removeModifiedTiles();

    om::event::Redraw2dBlocking();
  }

 private:
  static void removeModifiedTiles() {
    for (auto& seg : SegmentationDataWrapper::GetPtrVec()) {
      seg->SliceCache().Clear();
    }

    OmTileCache::ClearSegmentation();
  }
};

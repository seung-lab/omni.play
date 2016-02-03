#pragma once
#include "precomp.h"

#include "common/common.h"
#include "coordinates/global.h"
#include "common/enums.hpp"
#include "common/point2d.hpp"
#include "volume/omSegmentation.h"

class OmSegmentation;

namespace om {
typedef std::deque<point2di> pt2d_list_t;
typedef std::deque<om::coords::Global> pt3d_list_t;
};  // namespace om

struct OmBrushOppInfo {
  OmSegmentation& segmentation;
  const om::common::ViewType viewType;
  const int brushDia;
  const int depth;
  const std::vector<om::point2di>& ptsInCircle;
  const om::common::AddOrSubtract addOrSubtract;

  OmBrushOppInfo(OmSegmentation& segmentation,
                 const om::common::ViewType viewType, const int brushDia,
                 const int depth, const std::vector<om::point2di>& ptsInCircle,
                 om::common::AddOrSubtract addOrSubtract)
      : segmentation(segmentation),
        viewType(viewType),
        brushDia(brushDia),
        depth(depth),
        ptsInCircle(ptsInCircle),
        addOrSubtract(addOrSubtract) {}

  friend std::ostream& operator<<(std::ostream& out, const OmBrushOppInfo& i);
};

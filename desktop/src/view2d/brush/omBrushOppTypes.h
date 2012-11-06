#pragma once

#include "common/common.h"

class OmSegmentation;

namespace om {
typedef std::deque<point2di> pt2d_list_t;
typedef std::deque<om::coords::Global> pt3d_list_t;
}; //namespace om

struct OmBrushOppInfo
{
    OmSegmentation *const segmentation;
    const om::common::ViewType viewType;
    const int brushDia;
    const int depth;
    const std::vector<Vector2i>& ptsInCircle;
    const om::common::AddOrSubtract addOrSubract;

    OmBrushOppInfo(OmSegmentation* segmentation,
                   const om::common::ViewType viewType,
                   const int brushDia,
                   const int depth,
                   const std::vector<Vector2i>& ptsInCircle,
                   om::common::AddOrSubtract addOrSubract)
        : segmentation(segmentation)
        , viewType(viewType)
        , brushDia(brushDia)
        , depth(depth)
        , ptsInCircle(ptsInCircle)
        , addOrSubract(addOrSubract)
    {}

    friend std::ostream& operator<<(std::ostream &out, const OmBrushOppInfo& i);
};


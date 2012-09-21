#pragma once

#include "common/om.hpp"
#include "common/omCommon.h"

class OmSegmentation;

namespace om {
typedef std::deque<point2di> pt2d_list_t;
typedef std::deque<om::globalCoord> pt3d_list_t;
}; //namespace om

struct OmBrushOppInfo
{
    OmSegmentation *const segmentation;
    const ViewType viewType;
    const int brushDia;
    const int depth;
    const std::vector<om::point2di>& ptsInCircle;
    const om::AddOrSubtract addOrSubract;

    OmBrushOppInfo(OmSegmentation* segmentation,
                   const ViewType viewType,
                   const int brushDia,
                   const int depth,
                   const std::vector<om::point2di>& ptsInCircle,
                   om::AddOrSubtract addOrSubract)
        : segmentation(segmentation)
        , viewType(viewType)
        , brushDia(brushDia)
        , depth(depth)
        , ptsInCircle(ptsInCircle)
        , addOrSubract(addOrSubract)
    {}

    friend std::ostream& operator<<(std::ostream &out, const OmBrushOppInfo& i);
};


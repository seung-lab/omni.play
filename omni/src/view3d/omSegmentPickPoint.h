#ifndef OM_SEGMENT_PICK_POINT_HPP
#define OM_SEGMENT_PICK_POINT_HPP

#include "common/omCommon.h"
#include "utility/segmentDataWrapper.hpp"

struct OmSegmentPickPoint {
    SegmentDataWrapper sdw;
    DataCoord voxel;
};

#endif

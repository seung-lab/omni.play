#ifndef OM_SELECT_SEGMENT_PARAMS_HPP
#define OM_SELECT_SEGMENT_PARAMS_HPP

#include "common/om.hpp"
#include "common/omCommon.h"
#include "utility/dataWrappers.h"

struct OmSelectSegmentsParams {
    SegmentDataWrapper sdw;
    OmSegIDsSet newSelectedIDs;
    OmSegIDsSet oldSelectedIDs;
    void* sender;
    std::string comment;
    bool shouldScroll;
    bool addToRecentList;
    bool autoCenter;

    bool augmentListOnly;
    om::AddOrSubtract addOrSubtract;
};

#endif

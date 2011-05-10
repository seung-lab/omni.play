#pragma once

#include "common/om.hpp"
#include "common/omCommon.h"
#include "utility/dataWrappers.h"

struct OmSelectSegmentsParams
{
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

struct OmSegmentGUIparams
{
    SegmentationDataWrapper sdw;
};


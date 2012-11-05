#pragma once

#include "common/om.hpp"
#include "common/common.h"
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
    om::common::AddOrSubtract addOrSubtract;
};

struct OmSegmentGUIparams
{
    SegmentationDataWrapper sdw;
    bool stayOnPage;
};


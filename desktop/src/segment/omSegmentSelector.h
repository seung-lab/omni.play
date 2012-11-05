#pragma once

#include "common/common.h"

class OmSegmentation;
class OmSegments;
class OmSelectSegmentsParams;
class SegmentationDataWrapper;

class OmSegmentSelector {
public:
    OmSegmentSelector(const SegmentationDataWrapper& sdw,
                      void* sender,
                      const std::string& cmt);

    void selectJustThisSegment(const om::common::SegID segID, const bool isSelected);
    void augmentSelectedSet(const om::common::SegID segID, const bool isSelected);

    void InsertSegments(const boost::unordered_set<om::common::SegID>* segIDs);
    void RemoveSegments(const boost::unordered_set<om::common::SegID>* segIDs);

    void selectJustThisSegment_toggle(const om::common::SegID segID);
    void augmentSelectedSet_toggle(const om::common::SegID segID);

    bool sendEvent();
    void selectNoSegments();

    void ShouldScroll(const bool shouldScroll);
    void AddToRecentList(const bool addToRecentList);
    void AutoCenter(const bool autoCenter);
    void AugmentListOnly(const bool augmentListOnly);
    void AddOrSubtract(const om::common::AddOrSubtract addSegments);

private:
    OmSegments* segments_;
    om::shared_ptr<OmSelectSegmentsParams> params_;

    void setSelectedSegment(const om::common::SegID segID);
};


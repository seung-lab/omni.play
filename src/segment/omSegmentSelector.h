#pragma once

#include "common/common.h"

class segmentation;
class segments;
class OmSelectSegmentsParams;
class SegmentationDataWrapper;

class OmSegmentSelector {
public:
    OmSegmentSelector(const SegmentationDataWrapper& sdw,
                      void* sender,
                      const std::string& cmt);

    void selectJustThisSegment(const segId segID, const bool isSelected);
    void augmentSelectedSet(const segId segID, const bool isSelected);

    void InsertSegments(const boost::unordered_set<segId>* segIDs);
    void RemoveSegments(const boost::unordered_set<segId>* segIDs);

    void selectJustThisSegment_toggle(const segId segID);
    void augmentSelectedSet_toggle(const segId segID);

    bool sendEvent();
    void selectNoSegments();

    void ShouldScroll(const bool shouldScroll);
    void AddToRecentList(const bool addToRecentList);
    void AutoCenter(const bool autoCenter);
    void AugmentListOnly(const bool augmentListOnly);
    void AddOrSubtract(const om::AddOrSubtract addSegments);

private:
    segments* segments_;
    om::shared_ptr<OmSelectSegmentsParams> params_;

    void setSelectedSegment(const segId segID);
};


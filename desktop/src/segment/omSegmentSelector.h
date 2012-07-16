#pragma once

#include "common/omCommon.h"
#include "segment/io/omMSTtypes.h"

class OmSegmentation;
class OmSegments;
class OmSelectSegmentsParams;
class SegmentationDataWrapper;
class OmSegmentGraph;

class OmSegmentSelector {
public:
    OmSegmentSelector(const SegmentationDataWrapper& sdw,
                      void* sender,
                      const std::string& cmt);

    void setOrderOfAdditionToZero(OmSegID segID);
    void setOrderOfAdditionToNextNumber(OmSegID segID);

    void selectJustThisSegment(const OmSegID segID, const bool isSelected);
    void augmentSelectedSet(const OmSegID segID, const bool isSelected);

    void InsertSegments(const boost::unordered_set<OmSegID>* segIDs);
    void RemoveSegments(const boost::unordered_set<OmSegID>* segIDs);

    void selectJustThisSegment_toggle(const OmSegID segID);
    void augmentSelectedSet_toggle(const OmSegID segID);

    bool sendEvent();
    bool IsSegmentSelected(const OmSegID segID);
    void selectNoSegments();

    void ShouldScroll(const bool shouldScroll);
    void AddToRecentList(const bool addToRecentList);
    void AutoCenter(const bool autoCenter);
    void AugmentListOnly(const bool augmentListOnly);
    void AddOrSubtract(const om::AddOrSubtract addSegments);

private:
    uint32_t numberOfAddedSegment;
    OmSegments* segments_;
    om::shared_ptr<OmSelectSegmentsParams> params_;

    void setSelectedSegment(const OmSegID segID);
};


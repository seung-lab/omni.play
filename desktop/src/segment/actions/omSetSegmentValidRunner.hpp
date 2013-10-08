#pragma once

#include "segment/omSegmentUtils.hpp"
#include "system/omLocalPreferences.hpp"

class OmSetSegmentValidRunner {
 private:
  const SegmentDataWrapper sdw_;
  const om::SetValid validEnum_;

  OmSegID nextSegmentIDtoJumpTo_;

 public:
  OmSetSegmentValidRunner(const SegmentDataWrapper& sdw,
                          const om::SetValid validEnum)
      : sdw_(sdw), validEnum_(validEnum) {
    nextSegmentIDtoJumpTo_ = OmSegmentUtils::GetNextSegIDinWorkingList(sdw);
  }

  void Validate() {
    bool valid = false;
    if (om::SET_VALID == validEnum_) {
      valid = true;
    }

    OmSegIDsSet set;
    set.insert(sdw_.FindRootID());

    om::shared_ptr<std::set<OmSegment*> > children =
        OmSegmentUtils::GetAllChildrenSegments(sdw_.Segments(), set);

    (new OmSegmentValidateAction(sdw_.MakeSegmentationDataWrapper(), children,
                                 valid))->Run();
  }

  void JumpToNextSegment(const bool dontCenter) {
    OmSegments* segments = sdw_.Segments();

    const bool segmentGettingSetAsValid = om::SET_VALID == validEnum_;
    const bool shouldJump =
        OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();

    const bool justOneSegmentSelected =
        (1 == segments->NumberOfSelectedSegments());

    if (justOneSegmentSelected && segmentGettingSetAsValid && shouldJump &&
        nextSegmentIDtoJumpTo_ && !dontCenter) {
      OmSegmentSelector sel(sdw_.MakeSegmentationDataWrapper(), NULL,
                            "jump after validate");
      sel.selectJustThisSegment(nextSegmentIDtoJumpTo_, true);
      sel.AutoCenter(true);
      sel.sendEvent();
    }
  }
};

class OmSetSegmentsValidRunner {
 private:
  const SegmentationDataWrapper sdw_;
  const om::SetValid validEnum_;

  OmSegID nextSegmentIDtoJumpTo_;

 public:
  OmSetSegmentsValidRunner(const SegmentationDataWrapper& sdw,
                           const om::SetValid validEnum)
      : sdw_(sdw), validEnum_(validEnum) {
    nextSegmentIDtoJumpTo_ = OmSegmentUtils::GetNextSegIDinWorkingList(sdw);
  }

  void Validate() {
    bool valid = false;
    if (om::SET_VALID == validEnum_) {
      valid = true;
    }

    OmSegments* segments = sdw_.Segments();

    om::shared_ptr<std::set<OmSegment*> > children =
        OmSegmentUtils::GetAllChildrenSegments(
            segments, segments->GetSelectedSegmentIDs());

    (new OmSegmentValidateAction(sdw_, children, valid))->Run();
  }

  void JumpToNextSegment() {
    const bool segmentGettingSetAsValid = om::SET_VALID == validEnum_;
    const bool shouldJump =
        OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();

    if (shouldJump && segmentGettingSetAsValid && nextSegmentIDtoJumpTo_) {
      OmSegmentSelector sel(sdw_, NULL, "jump after validate");
      sel.selectJustThisSegment(nextSegmentIDtoJumpTo_, true);
      sel.AutoCenter(true);
      sel.sendEvent();
    }
  }
};

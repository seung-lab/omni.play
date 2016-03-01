#pragma once
#include "precomp.h"

#include "segment/omSegmentUtils.hpp"
#include "system/omLocalPreferences.hpp"
#include "segment/selection.hpp"

class OmSetSegmentValidRunner {
 private:
  const SegmentDataWrapper sdw_;
  const om::common::SetValid validEnum_;

  om::common::SegID nextSegmentIDtoJumpTo_;

 public:
  OmSetSegmentValidRunner(const SegmentDataWrapper& sdw,
                          const om::common::SetValid validEnum)
      : sdw_(sdw), validEnum_(validEnum) {
    nextSegmentIDtoJumpTo_ = OmSegmentUtils::GetNextSegIDinWorkingList(sdw);
  }

  void Validate() {
    if (!sdw_.IsSegmentValid()) {
      return;
    }

    bool valid = false;
    if (om::common::SetValid::SET_VALID == validEnum_) {
      valid = true;
    }

    om::common::SegIDSet set;
    set.insert(sdw_.FindRootID());

    std::shared_ptr<std::set<OmSegment*>> children =
        OmSegmentUtils::GetAllChildrenSegments(*sdw_.Segments(), set);

    (new OmSegmentValidateAction(sdw_.MakeSegmentationDataWrapper(), children,
                                 valid))->Run();
  }

  void JumpToNextSegment(const bool dontCenter) {
    OmSegments* segments = sdw_.Segments();

    const bool segmentGettingSetAsValid =
        om::common::SetValid::SET_VALID == validEnum_;
    const bool shouldJump =
        OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();

    const bool justOneSegmentSelected =
        (1 == segments->Selection().NumberOfSelectedSegments());

    if (justOneSegmentSelected && segmentGettingSetAsValid && shouldJump &&
        nextSegmentIDtoJumpTo_ && !dontCenter) {
      OmSegmentSelector sel(sdw_.MakeSegmentationDataWrapper(), nullptr,
                            "jump after validate");
      sel.selectJustThisSegment(nextSegmentIDtoJumpTo_, true);
      sel.AutoCenter(true);
    }
  }
};

class OmSetSegmentsValidRunner {
 private:
  const SegmentationDataWrapper sdw_;
  const om::common::SetValid validEnum_;

  om::common::SegID nextSegmentIDtoJumpTo_;

 public:
  OmSetSegmentsValidRunner(const SegmentationDataWrapper& sdw,
                           const om::common::SetValid validEnum)
      : sdw_(sdw), validEnum_(validEnum) {
    nextSegmentIDtoJumpTo_ = OmSegmentUtils::GetNextSegIDinWorkingList(sdw);
  }

  void Validate() {
    if (!sdw_.IsSegmentationValid()) {
      throw om::ArgException("Invalid SegmentationDataWrapper "
                             "(OmSetSegmentValidRunner::Validate)");
    }

    bool valid = false;
    if (om::common::SetValid::SET_VALID == validEnum_) {
      valid = true;
    }

    OmSegments* segments = sdw_.Segments();

    std::shared_ptr<std::set<OmSegment*>> children =
        OmSegmentUtils::GetAllChildrenSegments(
            *segments, segments->Selection().GetSelectedSegmentIDs());

    (new OmSegmentValidateAction(sdw_, children, valid))->Run();
  }

  void JumpToNextSegment(const bool dontCenter = false) {
    const bool segmentGettingSetAsValid =
        om::common::SetValid::SET_VALID == validEnum_;
    const bool shouldJump =
        OmLocalPreferences::GetShouldJumpToNextSegmentAfterValidate();

    if (shouldJump && segmentGettingSetAsValid && nextSegmentIDtoJumpTo_
        && !dontCenter) {
      OmSegmentSelector sel(sdw_, nullptr, "jump after validate");
      sel.selectJustThisSegment(nextSegmentIDtoJumpTo_, true);
      sel.AutoCenter(true);
    }
  }
};

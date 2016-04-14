#pragma once
#include "precomp.h"

#include "segment/actions/omJoinSplitRunner.hpp"
#include "gui/tools.hpp"
#include "viewGroup/omJoiningSplitting.hpp"
#include "viewGroup/omViewGroupState.h"
#include "segment/omSegment.h"
#include "utility/segmentationDataWrapper.hpp"
#include "common/enums.hpp"
#include "system/omStateManager.h"

// mouse release
void om::JoinSplitRunner::GoToNextState(OmViewGroupState& vgs,
    om::tool::mode tool) {
  OmJoiningSplitting& joiningSplitting = vgs.JoiningSplitting();

  joiningSplitting.GoToNextState();

  const om::common::SegIDSet firstBuffer = joiningSplitting.FirstBuffer();
  const om::common::SegIDSet secondBuffer = joiningSplitting.SecondBuffer();

  if (joiningSplitting.IsFinished() && !firstBuffer.empty()
      && !secondBuffer.empty()) {

    OmSegment* firstSegment = SegmentDataWrapper(vgs.Segmentation(),
        *joiningSplitting.FirstBuffer().begin()).GetSegment();
    OmSegment* secondSegment = SegmentDataWrapper(vgs.Segmentation(),
        *joiningSplitting.SecondBuffer().begin()).GetSegment();

    switch (tool) {
      case om::tool::mode::JOIN:
        OmActions::JoinSegments(
            vgs.Segmentation(), firstSegment, secondSegment);
        break;
      case om::tool::mode::SPLIT:
        OmActions::FindAndSplitSegments(
            vgs.Segmentation(), firstSegment, secondSegment);
        break;
      case om::tool::mode::MULTISPLIT:
        OmActions::FindAndSplitMultiSegments(
            vgs.Segmentation(), firstBuffer, secondBuffer);
        break;
    }
    //OmStateManager::SetOldToolModeAndSendEvent();
  }
}

//mouse click
void om::JoinSplitRunner::SelectSegment(OmViewGroupState& vgs,
    const om::tool::mode tool,
    const SegmentDataWrapper segmentDataWrapper) {
  if (!segmentDataWrapper.IsSegmentValid()) {
    return;
  }
  vgs.JoiningSplitting().SelectSegment(tool, segmentDataWrapper);
}

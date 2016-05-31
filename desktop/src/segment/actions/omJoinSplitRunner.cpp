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
#include "system/omLocalPreferences.hpp"

// mouse release
bool om::JoinSplitRunner::FinishSelectingSegments(OmViewGroupState& vgs,
    const om::tool::mode tool) {
  OmJoiningSplitting& joiningSplitting = *vgs.GetJoiningSplitting();

  // Finished state does not have any segments to select thus, an invalid cmd
  if (joiningSplitting.IsFinished()
      || joiningSplitting.RequiresElementSelection()) {
    return false;
  }

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
        OmActions::FindAndMultiSplitSegments(
            vgs.Segmentation(), firstBuffer, secondBuffer);
        break;
    }
    const bool shouldReturnOldTool =
      OmLocalPreferences::GetShouldReturnOldToolAfterJoinSplit();
    if (shouldReturnOldTool) {
      OmStateManager::SetOldToolModeAndSendEvent();
    }
  }
  return true;
}

//mouse click
bool om::JoinSplitRunner::SelectSegment(OmViewGroupState& vgs,
    const om::tool::mode tool,
    boost::optional<SegmentDataWrapper> segmentDataWrapper) {
  if (!segmentDataWrapper || !segmentDataWrapper->IsSegmentValid()) {
    return false;
  }
  vgs.GetJoiningSplitting()->SelectSegment(tool, *segmentDataWrapper);
  return true;
}

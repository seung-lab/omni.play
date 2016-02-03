#pragma once
#include "precomp.h"

#include "view2d/brush/omBrushOppInfo.hpp"
#include "view2d/brush/omBrushSelectCircle.hpp"
#include "view2d/brush/omBrushSelectLineTask.hpp"

namespace OmBrushSelect {
  void StartSelector(OmView2dState* state,
                            const om::coords::Global& coord,
                            const om::common::AddOrSubtract addSegments) {
    std::shared_ptr<OmSegmentSelector> selector = OmBrushSelectUtils::GetSelector(*state);

    std::shared_ptr<OmBrushOppInfo> info =
        OmBrushOppInfoFactory::MakeOppInfo(state, coord, addSegments);

    OmBrushSelectCircle circle(info, selector);

    circle.SelectCircle(coord);
  }

  void ContinueSelector(OmView2dState* state,
                           const om::coords::Global& second,
                           const om::common::AddOrSubtract addOrSubtract) {
    const om::coords::Global& first = state->GetLastDataPoint();

    std::shared_ptr<OmBrushOppInfo> info =
        OmBrushOppInfoFactory::MakeOppInfo(state, first, addOrSubtract);

    std::shared_ptr<OmSegmentSelector> selector = OmBrushSelectUtils::GetSelector(*state);
    std::shared_ptr<OmBrushSelectLineTask> task =
        std::make_shared<OmBrushSelectLineTask>(info, selector, first, second);

    OmView2dManager::AddTaskBack(task);
  }

  void EndSelector(OmView2dState* state) {
    state->getViewGroupState().EndSelector();
  }

  inline bool IsSelecting(OmView2dState* state) {
    return state->getViewGroupState().IsSelecting();
  }

};

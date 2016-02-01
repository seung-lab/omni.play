#pragma once
#include "precomp.h"

#include "view2d/brush/omBrushOppInfo.hpp"
#include "view2d/brush/omBrushSelectCircle.hpp"
#include "view2d/brush/omBrushSelectLineTask.hpp"
#include "viewGroup/omMultiSelecting.hpp"

class OmBrushSelect {
 public:
  static void SelectByClick(OmView2dState* state,
                            const om::coords::Global& coord,
                            const om::common::AddOrSubtract addSegments) {
    std::shared_ptr<OmBrushOppInfo> info =
        OmBrushOppInfoFactory::MakeOppInfo(state, coord, addSegments);

    OmBrushSelectCircle circle(info);

    circle.SelectCircle(coord);
  }

  static void StartOrContinueMultiSelecting(OmView2dState* state,
                           const om::coords::Global& second,
                           const om::common::AddOrSubtract addSegments) {
    const om::coords::Global& first = state->GetLastDataPoint();

    std::shared_ptr<OmBrushOppInfo> info =
        OmBrushOppInfoFactory::MakeOppInfo(state, first, addSegments);

    std::shared_ptr<OmBrushSelectLineTask> task =
        std::make_shared<OmBrushSelectLineTask>(info, first, second);

    OmView2dManager::AddTaskBack(task);
  }
};

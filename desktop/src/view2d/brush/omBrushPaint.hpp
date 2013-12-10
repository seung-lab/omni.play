#pragma once

#include "view2d/brush/omBrushOppInfo.hpp"
#include "view2d/brush/omBrushPaintCircle.hpp"
#include "view2d/brush/omBrushPaintLineTask.hpp"

class OmBrushPaint {
 public:
  static void PaintByClick(OmView2dState* state,
                           const om::coords::Global& coord,
                           const om::common::SegID segIDtoPaint) {
    std::shared_ptr<OmBrushOppInfo> info = OmBrushOppInfoFactory::MakeOppInfo(
        state, coord, om::common::AddOrSubtract::ADD);

    OmBrushPaintCircle circle(info, segIDtoPaint);

    circle.PaintCircle(coord);
  }

  static void PaintByLine(OmView2dState* state,
                          const om::coords::Global& second,
                          const om::common::SegID segIDtoPaint) {
    const om::coords::Global& first = state->GetLastDataPoint();

    std::shared_ptr<OmBrushOppInfo> info = OmBrushOppInfoFactory::MakeOppInfo(
        state, first, om::common::AddOrSubtract::ADD);

    std::shared_ptr<OmBrushPaintLineTask> task =
        std::make_shared<OmBrushPaintLineTask>(info, first, second,
                                               segIDtoPaint);

    OmView2dManager::AddTaskBack(task);
  }
};

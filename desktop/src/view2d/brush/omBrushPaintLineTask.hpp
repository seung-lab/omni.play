#pragma once

#include "zi/omThreads.h"
#include "view2d/brush/omBrushPaintLine.hpp"

class OmBrushPaintLineTask : public zi::runnable {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;
  const om::globalCoord first_;
  const om::globalCoord second_;
  const om::common::SegID segIDtoPaint_;

 public:
  OmBrushPaintLineTask(std::shared_ptr<OmBrushOppInfo> info,
                       const om::globalCoord& first,
                       const om::globalCoord& second,
                       const om::common::SegID segIDtoPaint)
      : info_(info),
        first_(first),
        second_(second),
        segIDtoPaint_(segIDtoPaint) {}

  void run() {
    OmBrushPaintLine line(info_, segIDtoPaint_);
    line.PaintLine(first_, second_);
  }
};

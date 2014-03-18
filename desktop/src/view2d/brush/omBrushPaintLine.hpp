#pragma once
#include "precomp.h"

#include "view2d/brush/omBrushOppLine.hpp"
#include "view2d/brush/omBrushPaintUtils.hpp"

class OmBrushPaintLine {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;
  const om::common::SegID segIDtoPaint_;

 public:
  OmBrushPaintLine(std::shared_ptr<OmBrushOppInfo> info,
                   const om::common::SegID segIDtoPaint)
      : info_(info), segIDtoPaint_(segIDtoPaint) {}

  virtual ~OmBrushPaintLine() {}

  void PaintLine(const om::coords::Global& first,
                 const om::coords::Global& second) {
    OmBrushOppLine lineOpp(info_);

    std::shared_ptr<om::pt3d_list_t> pts = lineOpp.GetPts(first, second);

    OmBrushPaintUtils::PaintPts(info_.get(), pts.get(), segIDtoPaint_);
  }
};

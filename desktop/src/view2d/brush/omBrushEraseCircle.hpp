#pragma once
#include "precomp.h"

#include "view2d/brush/omBrushEraseUtils.hpp"
#include "view2d/brush/omBrushOppCircle.hpp"

class OmBrushEraseCircle {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;
  const om::common::SegID segIDtoErase_;

 public:
  OmBrushEraseCircle(std::shared_ptr<OmBrushOppInfo> info,
                     const om::common::SegID segIDtoErase)
      : info_(info), segIDtoErase_(segIDtoErase) {}

  virtual ~OmBrushEraseCircle() {}

  void EraseCircle(const om::coords::Global& coord) {
    OmBrushOppCircle circleOpp(info_);

    std::shared_ptr<om::pt3d_list_t> pts = circleOpp.GetPts(coord);

    OmBrushEraseUtils::ErasePts(info_.get(), pts.get(), segIDtoErase_);
  }
};

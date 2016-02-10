#pragma once
#include "precomp.h"

#include "view2d/brush/omBrushSelectUtils.hpp"
#include "view2d/brush/omBrushOppCircle.hpp"

class OmBrushSelectCircle {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;
  const std::shared_ptr<OmSegmentSelector> selector_;

 public:
  OmBrushSelectCircle(std::shared_ptr<OmBrushOppInfo> info,
                      std::shared_ptr<OmSegmentSelector> selector)
   : info_(info), selector_(selector) {}

  virtual ~OmBrushSelectCircle() {}

  void SelectCircle(const om::coords::Global& coord) {
    OmBrushOppCircle circleOpp(info_);

    std::shared_ptr<om::pt3d_list_t> pts = circleOpp.GetPts(coord);

    om::common::SegIDSet segIDs =
        OmBrushSelectUtils::FindSegIDsFromPoints(*info_, *pts);

    OmBrushSelectUtils::AddOrSubtractSegments(*selector_, info_->addOrSubtract, segIDs);
  }
};

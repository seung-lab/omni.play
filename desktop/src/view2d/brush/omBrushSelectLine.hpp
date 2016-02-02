#pragma once
#include "precomp.h"

#include "view2d/brush/omBrushOppLine.hpp"
#include "view2d/brush/omBrushOppTypes.h"
#include "segment/omSegmentSelector.h"

class OmBrushSelectLine {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;
  const std::shared_ptr<OmSegmentSelector> selector_;

 public:
  OmBrushSelectLine(std::shared_ptr<OmBrushOppInfo> info,
                    std::shared_ptr<OmSegmentSelector> selector)
    : info_(info), selector_(selector) {}

  void SelectLine(const om::coords::Global& first,
                  const om::coords::Global& second) {
    OmBrushOppLine lineOpp(info_);

    std::shared_ptr<om::pt3d_list_t> pts = lineOpp.GetPts(first, second);

    om::common::SegIDSet segIDs =
        OmBrushSelectUtils::FindSegIDsFromPoints(*info_, *pts);

    OmBrushSelectUtils::AddOrSubtractSegments(*selector_, info_->addOrSubtract, segIDs);
  }
};

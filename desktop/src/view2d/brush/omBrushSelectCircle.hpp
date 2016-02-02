#pragma once
#include "precomp.h"

#include "view2d/brush/omBrushSelectUtils.hpp"
#include "view2d/brush/omBrushOppCircle.hpp"

class OmBrushSelectCircle {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;

 public:
  OmBrushSelectCircle(std::shared_ptr<OmBrushOppInfo> info) : info_(info) {}

  virtual ~OmBrushSelectCircle() {}

  void SelectCircle(const om::coords::Global& coord) {
    /* TODO::removeCircle investigate to see if we can do this
     * OmBrushOppCircle circleOpp(info_);

    std::shared_ptr<om::pt3d_list_t> pts = circleOpp.GetPts(coord);

    std::shared_ptr<om::common::SegIDSet> segIDs =
        OmBrushSelectUtils::FindSegIDsFromPoints(info_.get(), pts.get());

    OmBrushSelectUtils::SendEvent(info_.get(), segIDs.get());
    */
  }
};

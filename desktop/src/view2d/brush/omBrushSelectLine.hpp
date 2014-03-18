#pragma once
#include "precomp.h"

#include "view2d/brush/omBrushOppLine.hpp"
#include "view2d/brush/omBrushOppTypes.h"
#include "view2d/omSliceCache.hpp"
#include "segment/omSegmentSelector.h"

class OmBrushSelectLine {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;

 public:
  OmBrushSelectLine(std::shared_ptr<OmBrushOppInfo> info) : info_(info) {}

  virtual ~OmBrushSelectLine() {}

  void SelectLine(const om::coords::Global& first,
                  const om::coords::Global& second) {
    OmBrushOppLine lineOpp(info_);

    std::shared_ptr<om::pt3d_list_t> pts = lineOpp.GetPts(first, second);

    std::shared_ptr<om::common::SegIDSet> segIDs =
        OmBrushSelectUtils::FindSegIDsFromPoints(info_.get(), pts.get());

    OmBrushSelectUtils::SendEvent(info_.get(), segIDs.get());
  }
};

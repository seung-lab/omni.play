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

  //TODO::removePrintout
    std::cout << "Getting segments from " << first << " To " << second << std::endl;
    std::shared_ptr<om::pt3d_list_t> pts = lineOpp.GetPts(first, second);


  //TODO::removePrintout
    std::cout << "finding segIds for " << pts->size() << " points." << std::endl;
    om::common::SegIDSet segIDs =
        OmBrushSelectUtils::FindSegIDsFromPoints(*info_, *pts);

  //TODO::removePrintout
    std::cout << "found seg ids:";
    for (auto segID : segIDs) {
        std::cout << segID << " ";
    }
    std::cout << std::endl;
    OmBrushSelectUtils::AddOrSubtractSegments(*selector_, info_->addOrSubtract, segIDs);
  }
};

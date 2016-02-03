#pragma once
#include "precomp.h"

#include "view2d/brush/omBrushSelectLine.hpp"
#include "segment/omSegmentSelector.h"

class OmBrushSelectLineTask : public zi::runnable {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;
  const std::shared_ptr<OmSegmentSelector> selector_;
  const om::coords::Global first_;
  const om::coords::Global second_;

 public:
  OmBrushSelectLineTask(std::shared_ptr<OmBrushOppInfo> info,
                        std::shared_ptr<OmSegmentSelector> selector,
                        const om::coords::Global& first,
                        const om::coords::Global& second)
      : info_(info), selector_(selector), first_(first), second_(second) {}

  void run() {
    OmBrushSelectLine line(info_, selector_);
    line.SelectLine(first_, second_);
  }
};

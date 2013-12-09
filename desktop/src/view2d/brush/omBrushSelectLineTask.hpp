#pragma once

#include "zi/omThreads.h"
#include "view2d/brush/omBrushSelectLine.hpp"

class OmBrushSelectLineTask : public zi::runnable {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;
  const om::globalCoord first_;
  const om::globalCoord second_;

 public:
  OmBrushSelectLineTask(std::shared_ptr<OmBrushOppInfo> info,
                        const om::globalCoord& first,
                        const om::globalCoord& second)
      : info_(info), first_(first), second_(second) {}

  void run() {
    OmBrushSelectLine line(info_);
    line.SelectLine(first_, second_);
  }
};

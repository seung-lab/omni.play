#pragma once

#include "zi/omThreads.h"
#include "view2d/brush/omBrushSelectLine.hpp"

class OmBrushSelectLineTask : public zi::runnable {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;
  const om::coords::Global first_;
  const om::coords::Global second_;

 public:
  OmBrushSelectLineTask(std::shared_ptr<OmBrushOppInfo> info,
                        const om::coords::Global& first,
                        const om::coords::Global& second)
      : info_(info), first_(first), second_(second) {}

  void run() {
    OmBrushSelectLine line(info_);
    line.SelectLine(first_, second_);
  }
};

#pragma once
#include "precomp.h"

#include "view2d/brush/omBrushEraseLine.hpp"

class OmBrushEraseLineTask : public zi::runnable {
 private:
  const std::shared_ptr<OmBrushOppInfo> info_;
  const om::coords::Global first_;
  const om::coords::Global second_;
  const om::common::SegID segIDtoErase_;

 public:
  OmBrushEraseLineTask(std::shared_ptr<OmBrushOppInfo> info,
                       const om::coords::Global& first,
                       const om::coords::Global& second,
                       const om::common::SegID segIDtoErase)
      : info_(info),
        first_(first),
        second_(second),
        segIDtoErase_(segIDtoErase) {}

  void run() {
    OmBrushEraseLine line(info_, segIDtoErase_);
    line.EraseLine(first_, second_);
  }
};

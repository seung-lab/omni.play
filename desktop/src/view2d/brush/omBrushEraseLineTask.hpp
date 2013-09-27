#pragma once

#include "zi/omThreads.h"
#include "view2d/brush/omBrushEraseLine.hpp"

class OmBrushEraseLineTask : public zi::runnable {
private:
    const std::shared_ptr<OmBrushOppInfo> info_;
    const om::globalCoord first_;
    const om::globalCoord second_;
    const om::common::SegID segIDtoErase_;

public:
    OmBrushEraseLineTask(std::shared_ptr<OmBrushOppInfo> info,
                         const om::globalCoord& first,
                         const om::globalCoord& second,
                         const om::common::SegID segIDtoErase)
        : info_(info)
        , first_(first)
        , second_(second)
        , segIDtoErase_(segIDtoErase)
    {}

    void run()
    {
        OmBrushEraseLine line(info_, segIDtoErase_);
        line.EraseLine(first_, second_);
    }
};


#pragma once

#include "zi/omThreads.h"
#include "view2d/brush/omBrushSelectLine.hpp"

class OmBrushSelectLineTask : public zi::runnable {
private:
    const om::shared_ptr<OmBrushOppInfo> info_;
    const DataCoord first_;
    const DataCoord second_;

public:
    OmBrushSelectLineTask(om::shared_ptr<OmBrushOppInfo> info,
                          const DataCoord& first,
                          const DataCoord& second)
        : info_(info)
        , first_(first)
        , second_(second)
    {}

    void run()
    {
        OmBrushSelectLine line(info_);
        line.SelectLine(first_, second_);
    }
};


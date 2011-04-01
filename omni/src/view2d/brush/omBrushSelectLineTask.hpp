#ifndef OM_BRUSH_SELECT_LINE_TASK_HPP
#define OM_BRUSH_SELECT_LINE_TASK_HPP

#include "zi/omThreads.h"
#include "view2d/brush/omBrushSelectLine.hpp"

class OmBrushSelectLineTask : public zi::runnable {
private:
    const boost::shared_ptr<OmBrushOppInfo> info_;
    const DataCoord first_;
    const DataCoord second_;

public:
    OmBrushSelectLineTask(boost::shared_ptr<OmBrushOppInfo> info,
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

#endif

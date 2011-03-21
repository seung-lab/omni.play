#ifndef OM_BRUSH_PAINT_LINE_TASK_HPP
#define OM_BRUSH_PAINT_LINE_TASK_HPP

#include "zi/omThreads.h"
#include "view2d/brush/omBrushPaintLine.hpp"

class OmBrushPaintLineTask : public zi::runnable {
private:
    const boost::shared_ptr<OmBrushOppInfo> info_;
    const DataCoord first_;
    const DataCoord second_;
    const OmSegID segIDtoPaint_;

public:
    OmBrushPaintLineTask(boost::shared_ptr<OmBrushOppInfo> info,
                         const DataCoord& first,
                         const DataCoord& second,
                         const OmSegID segIDtoPaint)
        : info_(info)
        , first_(first)
        , second_(second)
        , segIDtoPaint_(segIDtoPaint)
    {}

    void run()
    {
        OmBrushPaintLine line(info_, segIDtoPaint_);
        line.PaintLine(first_, second_);
    }
};

#endif

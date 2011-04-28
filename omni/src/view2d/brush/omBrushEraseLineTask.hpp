#ifndef OM_BRUSH_ERASE_LINE_TASK_HPP
#define OM_BRUSH_ERASE_LINE_TASK_HPP

#include "zi/omThreads.h"
#include "view2d/brush/omBrushEraseLine.hpp"

class OmBrushEraseLineTask : public zi::runnable {
private:
    const om::shared_ptr<OmBrushOppInfo> info_;
    const DataCoord first_;
    const DataCoord second_;
    const OmSegID segIDtoErase_;

public:
    OmBrushEraseLineTask(om::shared_ptr<OmBrushOppInfo> info,
                         const DataCoord& first,
                         const DataCoord& second,
                         const OmSegID segIDtoErase)
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

#endif

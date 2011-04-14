#ifndef OM_BRUSH_ERASE_HPP
#define OM_BRUSH_ERASE_HPP

#include "view2d/brush/omBrushOppInfo.hpp"
#include "view2d/brush/omBrushEraseCircle.hpp"
#include "view2d/brush/omBrushEraseLineTask.hpp"

class OmBrushErase {
public:
    static void EraseByClick(OmView2dState* state, const DataCoord& coord,
                             const OmSegID segIDtoErase)
    {
        om::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, coord, om::ADD);

        OmBrushEraseCircle circle(info, segIDtoErase);

        circle.EraseCircle(coord);
    }

    static void EraseByLine(OmView2dState* state, const DataCoord& second,
                            const OmSegID segIDtoErase)
    {
        const DataCoord& first = state->GetLastDataPoint();

        om::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, first, om::ADD);

        om::shared_ptr<OmBrushEraseLineTask> task =
            om::make_shared<OmBrushEraseLineTask>(info, first, second, segIDtoErase);

        OmView2dManager::AddTaskBack(task);
    }
};

#endif

#pragma once

#include "view2d/brush/omBrushOppInfo.hpp"
#include "view2d/brush/omBrushEraseCircle.hpp"
#include "view2d/brush/omBrushEraseLineTask.hpp"

class OmBrushErase {
public:
    static void EraseByClick(OmView2dState* state, const om::coords::Global& coord,
                             const OmSegID segIDtoErase)
    {
        om::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, coord, om::ADD);

        OmBrushEraseCircle circle(info, segIDtoErase);

        circle.EraseCircle(coord);
    }

    static void EraseByLine(OmView2dState* state, const om::coords::Global& second,
                            const OmSegID segIDtoErase)
    {
        const om::coords::Global& first = state->GetLastDataPoint();

        om::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, first, om::ADD);

        om::shared_ptr<OmBrushEraseLineTask> task =
            om::make_shared<OmBrushEraseLineTask>(info, first, second, segIDtoErase);

        OmView2dManager::AddTaskBack(task);
    }
};


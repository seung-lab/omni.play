#pragma once

#include "view2d/brush/omBrushOppInfo.hpp"
#include "view2d/brush/omBrushEraseCircle.hpp"
#include "view2d/brush/omBrushEraseLineTask.hpp"

class OmBrushErase {
public:
    static void EraseByClick(OmView2dState* state, const om::coords::Global& coord,
                             const om::common::SegID segIDtoErase)
    {
        boost::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, coord, om::common::ADD);

        OmBrushEraseCircle circle(info, segIDtoErase);

        circle.EraseCircle(coord);
    }

    static void EraseByLine(OmView2dState* state, const om::coords::Global& second,
                            const om::common::SegID segIDtoErase)
    {
        const om::coords::Global& first = state->GetLastDataPoint();

        boost::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, first, om::common::ADD);

        boost::shared_ptr<OmBrushEraseLineTask> task =
            om::make_shared<OmBrushEraseLineTask>(info, first, second, segIDtoErase);

        OmView2dManager::AddTaskBack(task);
    }
};


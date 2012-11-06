#pragma once

#include "view2d/brush/omBrushOppInfo.hpp"
#include "view2d/brush/omBrushSelectCircle.hpp"
#include "view2d/brush/omBrushSelectLineTask.hpp"

class OmBrushSelect {
public:
    static void SelectByClick(OmView2dState* state, const om::coords::Global& coord,
                              const om::common::AddOrSubtract addSegments)
    {
        // std::cout << "mouse click data coord: " << coord << "\n";

        boost::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, coord, addSegments);

        OmBrushSelectCircle circle(info);

        circle.SelectCircle(coord);
    }

    static void SelectByLine(OmView2dState* state, const om::coords::Global& second,
                             const om::common::AddOrSubtract addSegments)
    {
        const om::coords::Global& first = state->GetLastDataPoint();

        boost::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, first, addSegments);

        // std::cout << *info << ", " << first << ", " << second << "\n";

        boost::shared_ptr<OmBrushSelectLineTask> task =
            boost::make_shared<OmBrushSelectLineTask>(info, first, second);

        OmView2dManager::AddTaskBack(task);
    }
};


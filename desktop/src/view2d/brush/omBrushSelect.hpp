#pragma once

#include "view2d/brush/omBrushOppInfo.hpp"
#include "view2d/brush/omBrushSelectCircle.hpp"
#include "view2d/brush/omBrushSelectLineTask.hpp"

class OmBrushSelect {
public:
    static void SelectByClick(OmView2dState* state, const om::globalCoord& coord,
                              const om::common::AddOrSubtract addSegments)
    {
        // std::cout << "mouse click data coord: " << coord << "\n";

        std::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, coord, addSegments);

        OmBrushSelectCircle circle(info);

        circle.SelectCircle(coord);
    }

    static void SelectByLine(OmView2dState* state, const om::globalCoord& second,
                             const om::common::AddOrSubtract addSegments)
    {
        const om::globalCoord& first = state->GetLastDataPoint();

        std::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, first, addSegments);

        // std::cout << *info << ", " << first << ", " << second << "\n";

        std::shared_ptr<OmBrushSelectLineTask> task =
            std::make_shared<OmBrushSelectLineTask>(info, first, second);

        OmView2dManager::AddTaskBack(task);
    }
};


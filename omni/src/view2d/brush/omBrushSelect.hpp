#ifndef OM_BRUSH_SELECT_HPP
#define OM_BRUSH_SELECT_HPP

#include "view2d/brush/omBrushOppInfo.hpp"
#include "view2d/brush/omBrushSelectCircle.hpp"
#include "view2d/brush/omBrushSelectLineTask.hpp"

class OmBrushSelect {
public:
    static void SelectByClick(OmView2dState* state, const DataCoord& coord,
                              const om::AddOrSubtract addSegments)
    {
        om::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, coord, addSegments);

        // std::cout << *info << ", " << coord << "\n";

        OmBrushSelectCircle circle(info);

        circle.SelectCircle(coord);
    }

    static void SelectByLine(OmView2dState* state, const DataCoord& second,
                             const om::AddOrSubtract addSegments)
    {
        const DataCoord& first = state->GetLastDataPoint();

        om::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, first, addSegments);

        // std::cout << *info << ", " << first << ", " << second << "\n";

        om::shared_ptr<OmBrushSelectLineTask> task =
            om::make_shared<OmBrushSelectLineTask>(info, first, second);

        OmView2dManager::AddTaskBack(task);
    }
};

#endif

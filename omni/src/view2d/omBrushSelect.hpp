#ifndef OM_BRUSH_SELECT_HPP
#define OM_BRUSH_SELECT_HPP

#include "view2d/omBrushSelectLineTask.hpp"
#include "view2d/omBrushOpp.hpp"

class OmBrushSelect {
public:
    static void SelectByClick(OmView2dState* state, const DataCoord& coord,
                              const om::AddOrSubtract addSegments)
    {
        boost::shared_ptr<OmBrushSelectCircle> circle =
            OmBrushOpp::MakeCircleOpp<OmBrushSelectCircle>(state, coord);

        circle->SetAddOrSubtract(addSegments);

        circle->SelectAndSendEvents(coord);
    }

    static void SelectByLine(OmView2dState* state, const DataCoord& second,
                             const om::AddOrSubtract addSegments)
    {
        const DataCoord& first = state->GetLastDataPoint();

        boost::shared_ptr<OmBrushSelectCircle> circle =
            OmBrushOpp::MakeCircleOpp<OmBrushSelectCircle>(state, second);

        circle->SetAddOrSubtract(addSegments);

        boost::shared_ptr<OmBrushSelectLineTask> task =
            boost::make_shared<OmBrushSelectLineTask>(circle, first, second);

        OmView2dManager::AddTaskBack(task);
    }
};

#endif

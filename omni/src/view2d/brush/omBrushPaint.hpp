#ifndef OM_BRUSH_PAINT_HPP
#define OM_BRUSH_PAINT_HPP

#include "view2d/brush/omBrushOppInfo.hpp"
#include "view2d/brush/omBrushPaintCircle.hpp"
#include "view2d/brush/omBrushPaintLineTask.hpp"

class OmBrushPaint {
public:
    static void PaintByClick(OmView2dState* state, const DataCoord& coord,
                             const OmSegID segIDtoPaint)
    {
        boost::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, coord, om::ADD);

        OmBrushPaintCircle circle(info, segIDtoPaint);

        circle.PaintCircle(coord);
    }

    static void PaintByLine(OmView2dState* state, const DataCoord& second,
                            const OmSegID segIDtoPaint)
    {
        const DataCoord& first = state->GetLastDataPoint();

        boost::shared_ptr<OmBrushOppInfo> info =
            OmBrushOppInfoFactory::MakeOppInfo(state, first, om::ADD);

        boost::shared_ptr<OmBrushPaintLineTask> task =
            boost::make_shared<OmBrushPaintLineTask>(info, first, second, segIDtoPaint);

        OmView2dManager::AddTaskBack(task);
    }
};

#endif

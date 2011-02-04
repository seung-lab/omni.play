#ifndef OM_BRUSH_PAINT_HPP
#define OM_BRUSH_PAINT_HPP

#include "view2d/omBrushPaintCircle.hpp"
#include "view2d/omBrushPaintLineTask.hpp"
#include "view2d/omBrushOpp.hpp"

class OmBrushPaint {
public:
	static void PaintByClick(OmView2dState* state, const DataCoord& coord,
							 const OmSegID segIDtoPaint)
	{
		boost::shared_ptr<OmBrushPaintCircle> circle =
			OmBrushOpp::MakeCircleOpp<OmBrushPaintCircle>(state, coord);

		circle->SetSegmentID(segIDtoPaint);

		circle->PaintCircleAndUpdate(coord);
	}

	static void PaintByLine(OmView2dState* state, const DataCoord& second,
							const OmSegID segIDtoPaint)
	{
		const DataCoord& first = state->GetLastDataPoint();

		boost::shared_ptr<OmBrushPaintCircle> circle =
			OmBrushOpp::MakeCircleOpp<OmBrushPaintCircle>(state, second);

		circle->SetSegmentID(segIDtoPaint);

		boost::shared_ptr<OmBrushPaintLineTask> task =
			boost::make_shared<OmBrushPaintLineTask>(circle, first, second);

		OmView2dManager::AddTaskBack(task);
	}
};

#endif

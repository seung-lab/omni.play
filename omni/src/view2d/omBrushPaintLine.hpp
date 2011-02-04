#ifndef OM_BRUSH_PAINT_LINE_HPP
#define OM_BRUSH_PAINT_LINE_HPP

#include "view2d/omBrushPaintCircle.hpp"
#include "view2d/omBrushOppLine.hpp"

class OmBrushPaintLine : private OmBrushOppLine {
private:
	const boost::shared_ptr<OmBrushPaintCircle> circle_;

public:
	OmBrushPaintLine(boost::shared_ptr<OmBrushPaintCircle> circle)
		: OmBrushOppLine(circle)
		, circle_(circle)
	{}

	virtual ~OmBrushPaintLine()
	{}

	void PaintLine(const DataCoord& first, const DataCoord& second)
	{
		LineOpp(first, second);
		circle_->Update();
	}
};

#endif

#ifndef OM_BRUSH_PAINT_LINE_TASK_HPP
#define OM_BRUSH_PAINT_LINE_TASK_HPP

#include "zi/omThreads.h"
#include "view2d/omBrushPaintLine.hpp"

class OmBrushPaintLineTask : public zi::runnable {
private:
	boost::shared_ptr<OmBrushPaintCircle> circle_;
	const DataCoord first_;
	const DataCoord second_;

public:
	OmBrushPaintLineTask(boost::shared_ptr<OmBrushPaintCircle> circle,
						 const DataCoord& first,
						 const DataCoord& second)
		: circle_(circle)
		, first_(first)
		, second_(second)
	{}

	void run()
	{
		OmBrushPaintLine line(circle_);
		line.PaintLine(first_, second_);
	}
};

#endif

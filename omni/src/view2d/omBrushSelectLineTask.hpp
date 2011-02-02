#ifndef OM_BRUSH_SELECT_LINE_TASK_HPP
#define OM_BRUSH_SELECT_LINE_TASK_HPP

#include "zi/omThreads.h"
#include "view2d/omBrushSelectLine.hpp"

class OmBrushSelectLineTask : public zi::runnable {
private:
	boost::shared_ptr<OmBrushSelectCircle> circle_;
	const DataCoord first_;
	const DataCoord second_;

public:
	OmBrushSelectLineTask(boost::shared_ptr<OmBrushSelectCircle> circle,
						  const DataCoord& first,
						  const DataCoord& second)
		: circle_(circle)
		, first_(first)
		, second_(second)
	{}

	void run()
	{
		OmBrushSelectLine line(circle_);
		line.SelectLine(first_, second_);
	}
};

#endif

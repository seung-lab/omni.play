#ifndef OM_BRUSH_SELECT_LINE_HPP
#define OM_BRUSH_SELECT_LINE_HPP

#include "view2d/omBrushSelectCircle.hpp"
#include "view2d/omBrushOppLine.hpp"

class OmBrushSelectLine : private OmBrushOppLine {
private:
	const boost::shared_ptr<OmBrushSelectCircle> circle_;

public:
	OmBrushSelectLine(boost::shared_ptr<OmBrushSelectCircle> circle)
		: OmBrushOppLine(circle)
		, circle_(circle)
	{}

	virtual ~OmBrushSelectLine()
	{}

	void SelectLine(const DataCoord& first, const DataCoord& second)
	{
		LineOpp(first, second);
		circle_->SendEvents();
	}
};

#endif

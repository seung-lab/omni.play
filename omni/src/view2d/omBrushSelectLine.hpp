#ifndef OM_BRUSH_SELECT_LINE_HPP
#define OM_BRUSH_SELECT_LINE_HPP

#include "view2d/omBrushSelectCircle.hpp"
#include "view2d/omView2dConverters.hpp"

class OmBrushSelectLine {
private:
	boost::shared_ptr<OmBrushSelectCircle> circle_;
	const ViewType viewType_;
	const int brushDia_;

public:
	OmBrushSelectLine(boost::shared_ptr<OmBrushSelectCircle> circle)
		: circle_(circle)
		, viewType_(circle->GetViewType())
		, brushDia_(circle->BrushDiameter())
	{}

	virtual ~OmBrushSelectLine()
	{}

	void SelectLine(const DataCoord& first, const DataCoord& second)
	{
		const Vector2i pt0 = OmView2dConverters::Get2PtsInPlane(first, viewType_);
		const Vector2i pt1 = OmView2dConverters::Get2PtsInPlane(second, viewType_);

		doBresenhamLineDraw(pt0.x, pt0.y, pt1.x, pt1.y);

		circle_->SendEvents();
	}

private:
	void doBresenhamLineDraw(int x0, int y0, int x1, int y1)
	{
		int dy = y1 - y0;
		int stepy = 1;
		if (dy < 0) {
			dy = -dy;
			stepy = -1;
		}

		int dx = x1 - x0;
		int stepx = 1;
		if (dx < 0) {
			dx = -dx;
			stepx = -1;
		}

		dy <<= 1;		// dy is now 2*dy
		dx <<= 1;		// dx is now 2*dx

		if (dx > dy) {
			int fraction = dy - (dx >> 1);	// same as 2*dy - dx
			while (x0 != x1) {
				if (fraction >= 0) {
					y0 += stepy;
					fraction -= dx;	// same as fraction -= 2*dx
				}
				x0 += stepx;
				fraction += dy;	// same as fraction -= 2*dy

				if(brushDia_ > 4 &&
				   (x1 == x0 || abs(x1 - x0) % (brushDia_ / 4) == 0))
				{
					circle_->SelectSegments(x0, y0);

				} else if (brushDia_ < 4) {
					circle_->SelectSegments(x0, y0);
				}
			}
		} else {
			int fraction = dx - (dy >> 1);
			while (y0 != y1) {
				if (fraction >= 0) {
					x0 += stepx;
					fraction -= dy;
				}

				y0 += stepy;
				fraction += dx;

				if(brushDia_ > 4 &&
				   (y1 == y0 || abs(y1 - y0) % (brushDia_ / 4) == 0))
				{
					circle_->SelectSegments(x0, y0);

				} else if (brushDia_ < 4) {
					circle_->SelectSegments(x0, y0);
				}
			}
		}
	}
};

#endif

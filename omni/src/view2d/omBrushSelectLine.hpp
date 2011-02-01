#ifndef OM_BRUSH_SELECT_LINE_HPP
#define OM_BRUSH_SELECT_LINE_HPP

#include "view2d/omBrushSelectCircle.hpp"

class OmBrushSelectLine : private OmBrushSelectCircle {
public:
	OmBrushSelectLine(OmSegmentation * segmentation,
					  const ViewType viewType,
					  const int brushDia,
					  const int depth)
		: OmBrushSelectCircle(segmentation, viewType, brushDia, depth)
	{}

	virtual ~OmBrushSelectLine()
	{}

	void SelectLine(const DataCoord& first, const DataCoord& second)
	{
		const TwoPoints pts = computeTwoPoints(first, second);
		doBresenhamLineDraw(pts.y0, pts.y1, pts.x0, pts.x1);

		sendEvents();
	}

private:
	struct TwoPoints {
		int x0;
		int x1;
		int y0;
		int y1;
	};

	TwoPoints computeTwoPoints(const DataCoord& first, const DataCoord& second)
	{
		switch(viewType_) {
		case XY_VIEW:
		{
			TwoPoints ret = {first.x,
							 second.x,
							 first.y,
							 second.y};
			return ret;
		}
		case XZ_VIEW:
		{
			TwoPoints ret = {first.x,
							 second.x,
							 first.z,
							 second.z};
			return ret;
		}
		case YZ_VIEW:
		{
			TwoPoints ret = {first.z,
							 second.z,
							 first.y,
							 second.y};
			return ret;
		}
		default:
			throw OmArgException("unknown type");
		}
	}

	void doBresenhamLineDraw(int y0, int y1, int x0, int x1)
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
					selectSegments(x0, y0);

				} else if (brushDia_ < 4) {
					selectSegments(x0, y0);
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
					selectSegments(x0, y0);

				} else if (brushDia_ < 4) {
					selectSegments(x0, y0);
				}
			}
		}
	}

	inline void selectSegments(const int x, const int y){
		OmBrushSelectCircle::selectSegments(DataCoord(x, y, depth_));
	}
};

#endif

#ifndef OM_BRUSH_SELECT_HPP
#define OM_BRUSH_SELECT_HPP

#include "viewGroup/omBrushSize.hpp"
#include "project/omProject.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"
#include "tiles/cache/omTileCache.h"
#include "system/omStateManager.h"
#include "viewGroup/omViewGroupState.h"
#include "utility/dataWrappers.h"
#include "view2d/omLineDraw.hpp"
#include "view2d/omView2dState.hpp"
#include "volume/omSegmentation.h"
#include "actions/omActions.hpp"
#include "common/omBoost.h"

class OmBrushSelect {
private:
	OmView2dState *const state_;
	const ViewType mViewType;
	const SegmentationDataWrapper sdw_;
	OmSegmentation *const segmentation_;

	const int brushDia_;
	int depth_;

	boost::scoped_ptr<OmSegmentSelector> selector_;

public:
	static void SelectByClick(OmView2dState* state, const DataCoord& gDC)
	{
		OmBrushSelect s(state);
		s.SelectSegments(gDC);
	}

	static void SelectByLine(OmView2dState* state,
							 const DataCoord& first,
							 const DataCoord& second)
	{
		OmBrushSelect s(state);
		s.BresenhamLineDrawForSelecting(first, second);
	}

private:
	OmBrushSelect(OmView2dState* state)
		: state_(state)
		, mViewType(state->getViewType())
		, sdw_(SegmentationDataWrapper(state->GetSegmentationID()))
		, segmentation_(sdw_.GetSegmentationPtr())
		, brushDia_(state_->getBrushSize()->Diameter())
		, depth_(0)
	{}

	virtual ~OmBrushSelect()
	{}

	void BresenhamLineDrawForSelecting(const DataCoord& first,
									   const DataCoord& second)
	{
		selector_.reset(new OmSegmentSelector(sdw_, this, "view2d_selector"));

		depth_ = OmView2dConverters::GetViewTypeDepth(second, mViewType);

		const TwoPoints pts = computeTwoPoints(first, second);
		doBresenhamLineDraw(pts.y0, pts.y1, pts.x0, pts.x1);

		selector_->sendEvent();
	}

	void SelectSegments(const DataCoord& gDC)
	{
		selector_.reset(new OmSegmentSelector(sdw_, this, "view2d_selector"));
		selectSegments(gDC);
		selector_->sendEvent();
	}

	struct TwoPoints {
		int x0;
		int x1;
		int y0;
		int y1;
	};

	TwoPoints computeTwoPoints(const DataCoord& first, const DataCoord& second)
	{
		TwoPoints ret = {0, 0, 0, 0};

		switch (mViewType) {
		case XY_VIEW:
			ret.x0 = first.x;
			ret.x1 = second.x;
			ret.y0 = first.y;
			ret.y1 = second.y;
			break;
		case XZ_VIEW:
			ret.x0 = first.x;
			ret.x1 = second.x;
			ret.y0 = first.z;
			ret.y1 = second.z;
			break;
		case YZ_VIEW:
			ret.x0 = first.z;
			ret.x1 = second.z;
			ret.y0 = first.y;
			ret.y1 = second.y;
			break;
		}

		return ret;
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

	inline void addVoxel(const DataCoord& xyzCoord)
	{
		if( xyzCoord.x < 0 ||
			xyzCoord.y < 0 ||
			xyzCoord.z < 0)
		{
			return;
		}

		const DataCoord viewtypeCoord = getCoordsByViewType(xyzCoord);

		const OmSegID segID =
			segmentation_->GetVoxelValue(viewtypeCoord);

		if(segID) {
			selector_->augmentSelectedSet(segID, true);
		}
	}

	inline void selectSegments(const int x, const int y){
		selectSegments(DataCoord(x, y, depth_));
	}

	inline void selectSegments(const DataCoord& xyzCoord)
	{
		if(1 == brushDia_){
			addVoxel(xyzCoord);
			return;
		}

		selectSegmentsInCircle(xyzCoord);
	}

	inline DataCoord getCoordsByViewType(const DataCoord& vec){
		return OmView2dConverters::MakeViewTypeVector3(vec, mViewType);
	}

	inline void selectSegmentsInCircle(const DataCoord& xyzCoord)
	{
        const int radius   = brushDia_ / 2;
        const int sqRadius = radius * radius;

        for(int i = 0; i < brushDia_; ++i)
		{
			const int x = i - radius;

            for(int j = 0; j < brushDia_; ++j)
            {
                const int y = j - radius;

                if( x * x + y * y <= sqRadius )
                {
                    addVoxel(DataCoord(xyzCoord.x + x,
									   xyzCoord.y + y,
									   xyzCoord.z));
                }
            }
        }
	}
};

#endif

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

	boost::scoped_ptr<OmSegmentSelector> selector_;

public:
	OmBrushSelect(OmView2dState* state)
		: state_(state)
		, mViewType(state->getViewType())
		, sdw_(SegmentationDataWrapper(state->GetSegmentationID()))
		, segmentation_(sdw_.GetSegmentationPtr())
	{}

	void BresenhamLineDrawForSelecting(const DataCoord& first,
									   const DataCoord& second)
	{
		selector_.reset(new OmSegmentSelector(sdw_, this, "view2d_selector"));

		const TwoPoints pts = computeTwoPoints(first, second);
		const int depth = OmView2dConverters::GetViewTypeDepth(second, mViewType);

		doBresenhamLineDraw(pts.y0, pts.y1, pts.x0, pts.x1, depth);

		selector_->sendEvent();
	}

	void SelectSegments(const DataCoord& gDC)
	{
		selector_.reset(new OmSegmentSelector(sdw_, this, "view2d_selector"));
		selectSegments(gDC);
		selector_->sendEvent();
	}

private:
	struct TwoPoints {
		int x0;
		int y0;
		int x1;
		int y1;
	};

	TwoPoints computeTwoPoints(const DataCoord& first, const DataCoord& second)
	{
		TwoPoints ret = {0, 0, 0, 0};

		switch (mViewType) {
		case XY_VIEW:
			ret.y1 = second.y;
			ret.y0 = first.y;
			ret.x1 = second.x;
			ret.x0 = first.x;
			break;
		case XZ_VIEW:
			ret.y1 = second.z;
			ret.y0 = first.z;
			ret.x1 = second.x;
			ret.x0 = first.x;
			break;
		case YZ_VIEW:
			ret.y1 = second.y;
			ret.y0 = first.y;
			ret.x1 = second.z;
			ret.x0 = first.z;
			break;
		}

		return ret;
	}

	void doBresenhamLineDraw(int y0, int y1, int x0, int x1,
							 const int depth)
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

		const int brushDia = state_->getBrushSize()->Diameter();

		if (dx > dy) {
			int fraction = dy - (dx >> 1);	// same as 2*dy - dx
			while (x0 != x1) {
				if (fraction >= 0) {
					y0 += stepy;
					fraction -= dx;	// same as fraction -= 2*dx
				}
				x0 += stepx;
				fraction += dy;	// same as fraction -= 2*dy

				if(brushDia > 4 &&
				   (x1 == x0 || abs(x1 - x0) % (brushDia / 4) == 0))
				{
					const DataCoord globalDC =
						OmView2dConverters::MakeViewTypeVector3(x0, y0, depth, mViewType);

					selectSegments(globalDC);

				} else if (brushDia < 4) {
					const DataCoord globalDC =
						OmView2dConverters::MakeViewTypeVector3(x0, y0, depth, mViewType);

					selectSegments(globalDC);
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

				if(brushDia > 4 &&
				   (y1 == y0 || abs(y1 - y0) % (brushDia / 4) == 0))
				{
					const DataCoord globalDC =
						OmView2dConverters::MakeViewTypeVector3(x0, y0, depth, mViewType);
					selectSegments(globalDC);

				} else if (brushDia < 4) {
					const DataCoord globalDC =
						OmView2dConverters::MakeViewTypeVector3(x0, y0, depth, mViewType);

					selectSegments(globalDC);
				}
			}
		}
	}

	inline void addVoxel(const DataCoord& globalDataClickPoint)
	{
		if( globalDataClickPoint.x < 0 ||
			globalDataClickPoint.y < 0 ||
			globalDataClickPoint.z < 0)
		{
			return;
		}
		const OmSegID segID = segmentation_->GetVoxelValue(globalDataClickPoint);
		if(segID) {
			selector_->augmentSelectedSet(segID, true);
		}
	}

	inline void selectSegments(const DataCoord& gDC)
	{
		const int brushDia = state_->getBrushSize()->Diameter();
		if(1 == brushDia){
			addVoxel(gDC);
			return;
		}

		doSelectSegments(gDC, brushDia);
	}

	inline DataCoord brushToolToGDC(const DataCoord& vec)
	{
		return OmView2dConverters::MakeViewTypeVector3(vec.x,
													   vec.y,
													   vec.z,
													   mViewType);
	}

	inline void doSelectSegments(const DataCoord& gDC, const int savedDia)
	{
		const DataCoord off = brushToolToGDC(gDC);

        const int radius   = savedDia / 2;
        const int sqRadius = radius * radius;

        for(int i = 0; i < savedDia; ++i)
		{
			const int x = i - radius;

            for(int j = 0; j < savedDia; ++j)
            {
                const int y = j - radius;

                if( x * x + y * y <= sqRadius )
                {
                    addVoxel(brushToolToGDC( DataCoord( off.x + x,
														off.y + y )));
                }
            }
        }
	}

	void play(int dA, int dB, int A1, int A2, int B1, int B2,
			  const int stepA, const int setpB)
	{

	}
};

#endif

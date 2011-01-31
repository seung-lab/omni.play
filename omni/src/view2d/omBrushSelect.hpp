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
		doBresenhamLineDraw(first, second);
		selector_->sendEvent();
	}

	void SelectSegments(const DataCoord& gDC)
	{
		selector_.reset(new OmSegmentSelector(sdw_, this, "view2d_selector"));
		selectSegments(gDC);
		selector_->sendEvent();
	}

private:
	void doBresenhamLineDraw(const DataCoord& first, const DataCoord& second)
	{
		const float mDepth = state_->getViewGroupState()->GetViewSliceDepth(mViewType);
		const DataCoord data_coord = state_->getVol()->SpaceToDataCoord(SpaceCoord(0, 0, mDepth));

		DataCoord globalDC;
		int y1, y0, x1, x0;

		switch (mViewType) {
		case XY_VIEW:
			y1 = second.y;
			y0 = first.y;
			x1 = second.x;
			x0 = first.x;
			break;
		case XZ_VIEW:
			y1 = second.z;
			y0 = first.z;
			x1 = second.x;
			x0 = first.x;
			break;
		case YZ_VIEW:
			y1 = second.y;
			y0 = first.y;
			x1 = second.z;
			x0 = first.z;
			break;
		}

		int dy = y1 - y0;
		int dx = x1 - x0;
		int stepx, stepy;

		if (dy < 0) {
			dy = -dy;
			stepy = -1;
		} else {
			stepy = 1;
		}
		if (dx < 0) {
			dx = -dx;
			stepx = -1;
		} else {
			stepx = 1;
		}
		dy <<= 1;		// dy is now 2*dy
		dx <<= 1;		// dx is now 2*dx

		//debug(brush, "coords: %i,%i,%i\n", x0, y0, mViewDepth);
		//debug(brush, "mDepth = %f\n", mDepth);

		OmBrushSize* brushSize = state_->getBrushSize();

		if (dx > dy) {
			int fraction = dy - (dx >> 1);	// same as 2*dy - dx
			while (x0 != x1) {
				if (fraction >= 0) {
					y0 += stepy;
					fraction -= dx;	// same as fraction -= 2*dx
				}
				x0 += stepx;
				fraction += dy;	// same as fraction -= 2*dy

				// do the conversion ourselves; painting "skips" otherwise
				DataCoord globalDC;
				switch (mViewType) {
				case XY_VIEW:
					globalDC = DataCoord(x0, y0, second.z);
					break;
				case XZ_VIEW:
					globalDC = DataCoord(x0, second.y, y0);
					break;
				case YZ_VIEW:
					globalDC = DataCoord(second.x, y0, x0);
					break;
				}

				if(brushSize->Diameter() > 4 &&
				   (x1 == x0 || abs(x1 - x0) % (brushSize->Diameter() / 4) == 0))
				{
					selectSegments(globalDC);
				} else if (brushSize->Diameter() < 4) {
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

				// do the conversion ourselves; painting "skips" otherwise
				DataCoord globalDC;
				switch (mViewType) {
				case XY_VIEW:
					globalDC = DataCoord(x0, y0, second.z);
					break;
				case XZ_VIEW:
					globalDC = DataCoord(x0, second.y, y0);
					break;
				case YZ_VIEW:
					globalDC = DataCoord(second.x, y0, x0);
					break;
				}

				if(brushSize->Diameter() > 4 &&
				   (y1 == y0 || abs(y1 - y0) % (brushSize->Diameter() / 4) == 0))
				{
					selectSegments(globalDC);
				} else if (brushSize->Diameter() < 4) {
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

	inline DataCoord brushToolToGDC(const DataCoord& vec){
		return state_->makeViewTypeVector3(vec);
	}

	inline void doSelectSegments(const DataCoord& gDC, const int savedDia)
	{
		DataCoord off = brushToolToGDC(gDC);

		for (int i = 0; i < savedDia; i++) {
			for (int j = 0; j < savedDia; j++) {
				DataCoord myoff = off;
				float x = i - savedDia / 2.0;
				float y = j - savedDia / 2.0;
				if (sqrt(x * x + y * y) <= savedDia / 2.0) {
					myoff.x += i - savedDia / 2.0;
					myoff.y += j - savedDia / 2.0;

					addVoxel(brushToolToGDC(myoff));
				}
			}
		}
	}
};

#endif

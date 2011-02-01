#ifndef OM_BRUSH_SELECT_HPP
#define OM_BRUSH_SELECT_HPP

#include "segment/omSegmentSelector.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omBrushSize.hpp"
#include "volume/omSegmentation.h"

class OmBrushSelect {
private:
	OmView2dState *const state_;

	const ViewType viewType_;
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
		s.SendEvents();
	}

	static void SelectByLine(OmView2dState* state,
							 const DataCoord& first,
							 const DataCoord& second)
	{
		OmBrushSelect s(state);
		s.BresenhamLineDrawForSelecting(first, second);
		s.SendEvents();
	}

private:
	OmBrushSelect(OmView2dState* state)
		: state_(state)
		, viewType_(state->getViewType())
		, sdw_(SegmentationDataWrapper(state->GetSegmentationID()))
		, segmentation_(sdw_.GetSegmentationPtr())
		, brushDia_(state_->getBrushSize()->Diameter())
		, depth_(0)
	{
		if(brushDia_ > 1){
			if(0 != brushDia_%2){
				throw OmArgException("brush diameter must be a mulitple of 2");
			}
		}

		if(brushDia_ > 2){
			if(0 != brushDia_%4){
				throw OmArgException("brush diameter must be a mulitple of 4");
			}
		}
	}

	virtual ~OmBrushSelect()
	{}

	void BresenhamLineDrawForSelecting(const DataCoord& first,
									   const DataCoord& second)
	{
		setup();

		const TwoPoints pts = computeTwoPoints(first, second);
		doBresenhamLineDraw(pts.y0, pts.y1, pts.x0, pts.x1);
	}

	void SelectSegments(const DataCoord& coord)
	{
		setup();

		selectSegments(coord);
	}

	void SendEvents()
	{
		selector_->sendEvent();
		OmEvents::Redraw2d();
	}

private:
	void setup()
	{
		selector_.reset(new OmSegmentSelector(sdw_, this, "view2d_selector"));
		selector_->ShouldScroll(false);
		selector_->AddToRecentList(false);
		selector_->AutoCenter(false);

		depth_ = state_->getSliceDepth();
		std::cout << "slice depth is: " << depth_ << "\n";
	}

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

	inline void addVoxel(const DataCoord& xyzCoord)
	{
		if( xyzCoord.x < 0 ||
			xyzCoord.y < 0 ||
			xyzCoord.z < 0)
		{
			return;
		}

		const OmSegID segID = segmentation_->GetVoxelValue(xyzCoord);

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

	inline DataCoord flipCoord(const DataCoord& coord){
		return OmView2dConverters::MakeViewTypeVector3(coord, viewType_);
	}

	inline DataCoord flipCoord(const int x, const int y, const int z){
		return OmView2dConverters::MakeViewTypeVector3(x, y, z, viewType_);
	}

	inline void selectSegmentsInCircle(const DataCoord& origCoord)
	{
		const DataCoord flippedCoord = flipCoord(origCoord);

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
                    addVoxel(flipCoord(flippedCoord.x + x,
									   flippedCoord.y + y,
									   flippedCoord.z));
                }
            }
        }
	}
};

#endif

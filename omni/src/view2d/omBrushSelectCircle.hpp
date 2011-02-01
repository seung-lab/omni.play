#ifndef OM_BRUSH_SELECT_CIRCLE_HPP
#define OM_BRUSH_SELECT_CIRCLE_HPP

#include "segment/omSegmentSelector.h"
#include "system/omEvents.h"
#include "view2d/omView2dConverters.hpp"
#include "volume/omSegmentation.h"

class OmBrushSelectCircle {
private:
	OmSegmentation *const segmentation_;
	const ViewType viewType_;
	const int brushDia_;
	const int depth_;

	boost::scoped_ptr<OmSegmentSelector> selector_;

public:
	OmBrushSelectCircle(OmSegmentation * segmentation,
						const ViewType viewType,
						const int brushDia,
						const int depth)
		: segmentation_(segmentation)
		, viewType_(viewType)
		, brushDia_(brushDia)
		, depth_(depth)
	{
		setupSelector();
	}

	virtual ~OmBrushSelectCircle()
	{}

	void SelectAndSendEvents(const DataCoord& coord)
	{
		selectSegments(coord);
		SendEvents();
	}

	void SendEvents()
	{
		selector_->sendEvent();
		OmEvents::Redraw2d();
	}

	inline void SelectSegments(const int x, const int y){
		selectSegments(DataCoord(x, y, depth_));
	}

private:
	void setupSelector()
	{
		selector_.reset(new OmSegmentSelector(segmentation_->getSDW(),
											  this, "view2d_selector"));
		selector_->ShouldScroll(false);
		selector_->AddToRecentList(false);
		selector_->AutoCenter(false);
	}

	inline void selectSegments(const DataCoord& xyzCoord)
	{
		if(1 == brushDia_){
			addVoxel(xyzCoord);
			return;
		}

		selectSegmentsInCircle(xyzCoord);
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

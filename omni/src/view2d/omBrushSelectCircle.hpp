#ifndef OM_BRUSH_SELECT_CIRCLE_HPP
#define OM_BRUSH_SELECT_CIRCLE_HPP

#include "segment/omSegmentSelector.h"
#include "system/omEvents.h"
#include "view2d/omView2dManager.hpp"
#include "volume/omSegmentation.h"

class OmBrushSelectCircle {
private:
	OmSegmentation *const segmentation_;
	const ViewType viewType_;
	const int brushDia_;
	const int depth_;

	const std::vector<om::point2d>& ptsInCircle_;

	boost::scoped_ptr<OmSegmentSelector> selector_;
	boost::unordered_set<OmSegID> segIDs_;

public:
	OmBrushSelectCircle(OmSegmentation * segmentation,
						const ViewType viewType,
						const int brushDia,
						const int depth)
		: segmentation_(segmentation)
		, viewType_(viewType)
		, brushDia_(brushDia)
		, depth_(depth)
		, ptsInCircle_(OmView2dManager::GetPtsInCircle(brushDia))
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
		selector_->InsertSegments(segIDs_);
		selector_->sendEvent();
		OmEvents::Redraw2d();
	}

	inline void SelectSegments(const int x, const int y){
		selectSegments(DataCoord(x, y, depth_));
	}

	ViewType GetViewType() const {
		return viewType_;
	}

	int BrushDiameter() const {
		return brushDia_;
	}

private:
	void setupSelector()
	{
		selector_.reset(new OmSegmentSelector(segmentation_->getSDW(),
											  this, "view2d_selector"));
		selector_->ShouldScroll(false);
		selector_->AddToRecentList(false);
		selector_->AutoCenter(false);
		selector_->AugmentListOnly(true);
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
			segIDs_.insert(segID);
		}
	}

	inline void selectSegmentsInCircle(const DataCoord& origCoord)
	{
		FOR_EACH(iter, ptsInCircle_){
			addVoxel(DataCoord(origCoord.x + iter->x,
							   origCoord.y + iter->y,
							   origCoord.z));
		}
	}
};

#endif

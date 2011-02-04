#ifndef OM_BRUSH_PAINT_CIRCLE_HPP
#define OM_BRUSH_PAINT_CIRCLE_HPP

#include "actions/omActions.h"
#include "tiles/cache/omTileCache.h"
#include "view2d/omBrushOppCircle.hpp"

class OmBrushPaintCircle : public OmBrushOppCircle {
private:
	OmSegID segIDtoPaint_;

	std::set<DataCoord> updatedDataCoords_;
	bool doRefresh_;

public:
	OmBrushPaintCircle(OmSegmentation* segmentation,
					   const ViewType viewType,
					   const int brushDia,
					   const int depth,
					   const std::vector<om::point2d>& ptsInCircle)
		: OmBrushOppCircle(segmentation, viewType, brushDia, depth, ptsInCircle)
	{}

	virtual ~OmBrushPaintCircle()
	{}

	void SetSegmentID(const OmSegID segIDtoPaint){
		segIDtoPaint_ = segIDtoPaint;
	}

	void PaintCircleAndUpdate(const DataCoord& coord)
	{
		CircleOpp(coord);
		myUpdate();
	}

	void Update(){
		myUpdate();
	}

private:
	void myUpdate()
	{
		if(!doRefresh_){
			return;
		}

		OmActions::SetVoxels(segmentation_->GetID(),
							 updatedDataCoords_,
							 segIDtoPaint_);

		removeModifiedTiles();
	}

	void removeModifiedTiles()
	{
		std::set<DataCoord> dataCoordsToRemove;

		FOR_EACH(iter, updatedDataCoords_){
			dataCoordsToRemove.insert(*iter);
		}

		FOR_EACH(iter, dataCoordsToRemove){
			OmTileCache::RemoveDataCoord(*iter);
		}
	}

	inline void voxelOpp(const DataCoord& xyzCoord)
	{
		if( xyzCoord.x < 0 ||
			xyzCoord.y < 0 ||
			xyzCoord.z < 0)
		{
			return;
		}

		updatedDataCoords_.insert(xyzCoord);
		doRefresh_ = true;
	}
};

#endif

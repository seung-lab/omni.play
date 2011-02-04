#ifndef OM_BRUSH_OPP_CIRCLE_HPP
#define OM_BRUSH_OPP_CIRCLE_HPP

#include "view2d/omView2dConverters.hpp"
#include "volume/omSegmentation.h"

class OmBrushOppCircle {
protected:
	OmSegmentation *const segmentation_;
	const ViewType viewType_;
	const int brushDia_;
	const int depth_;

	const std::vector<om::point2d>& ptsInCircle_;

protected:
	OmBrushOppCircle(OmSegmentation* segmentation,
					 const ViewType viewType,
					 const int brushDia,
					 const int depth,
					 const std::vector<om::point2d>& ptsInCircle)
		: segmentation_(segmentation)
		, viewType_(viewType)
		, brushDia_(brushDia)
		, depth_(depth)
		, ptsInCircle_(ptsInCircle)
	{}

	virtual ~OmBrushOppCircle()
	{}

public:
	inline void CircleOpp(const int x, const int y){
		CircleOpp(DataCoord(x, y, depth_));
	}

	inline void CircleOpp(const DataCoord& xyzCoord)
	{
		if(1 == brushDia_){
			voxelOpp(xyzCoord);
			return;
		}

		const DataCoord flipped = flipCoords(xyzCoord);

		FOR_EACH(iter, ptsInCircle_){
			voxelOpp(flipCoords(flipped.x + iter->x,
								flipped.y + iter->y,
								flipped.z));
		}
	}

	ViewType GetViewType() const {
		return viewType_;
	}

	int BrushDiameter() const {
		return brushDia_;
	}

private:
	virtual void voxelOpp(const DataCoord& xyzCoord) = 0;

    inline DataCoord flipCoords(const int x, const int y, const int z) const {
        return OmView2dConverters::MakeViewTypeVector3(x, y, z, viewType_);
    }

    inline DataCoord flipCoords(const DataCoord& vec) const {
        return OmView2dConverters::MakeViewTypeVector3(vec, viewType_);
    }
};

#endif

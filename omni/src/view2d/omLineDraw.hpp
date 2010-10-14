#ifndef OM_LINE_DRAW_HPP
#define OM_LINE_DRAW_HPP

#include "common/omCommon.h"
#include "view2d/omView2dState.hpp"

class OmSegmentSelector;
class OmSegmentation;
class OmView2dState;

class OmLineDraw{
public:
	OmLineDraw(boost::shared_ptr<OmView2dState>, const ViewType);

	void BrushToolApplyPaint(OmId segid, DataCoord gDC, OmSegID seg);
	void bresenhamLineDraw(const DataCoord &first,
						   const DataCoord &second,
						   bool doselection);
	void FillToolFill(OmId segmentation,
					  DataCoord gCP,
					  OmSegID fc,
					  OmSegID bc,
					  int depth=0);

	void myUpdate();

private:
	boost::shared_ptr<OmView2dState> state_;
	const ViewType mViewType;
	bool mDoRefresh;
	OmId mEditedSegmentation;
	OmId mCurrentSegmentId;

	std::set<DataCoord> mUpdatedDataCoords;

	DataCoord BrushToolToGDC(const DataCoord& vec){
		return state_->makeViewTypeVector3(vec);
	}

	void RemoveModifiedTiles();
	void PickToolAddToSelection(OmSegmentSelector & sel,
								OmSegmentation & current_seg,
								DataCoord globalDataClickPoint);

};

#endif

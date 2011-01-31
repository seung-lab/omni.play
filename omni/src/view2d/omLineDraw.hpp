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

	void DrawLine(const DataCoord& startPoint,
				  const DataCoord& endPoint);

	void BrushToolApplyPaint(OmID segid, DataCoord gDC, OmSegID seg);

	void BresenhamLineDrawForPainting(const DataCoord & first,
									  const DataCoord & second);

	void BresenhamLineDrawForSelecting(const DataCoord & first,
									   const DataCoord & second);

	void FillToolFill(OmID segmentation,
					  DataCoord gCP,
					  OmSegID fc,
					  OmSegID bc,
					  int depth=0);

	void MyUpdate();

private:
	boost::shared_ptr<OmView2dState> state_;
	const ViewType mViewType;
	bool mDoRefresh;
	OmID mEditedSegmentation;
	OmID mCurrentSegmentId;

	std::set<DataCoord> mUpdatedDataCoords;

	DataCoord brushToolToGDC(const DataCoord& vec){
		return state_->makeViewTypeVector3(vec);
	}

	void removeModifiedTiles();
	void pickToolAddToSelection(OmSegmentSelector& sel,
								OmSegmentation& current_seg,
								const DataCoord& globalDataClickPoint);

};

#endif

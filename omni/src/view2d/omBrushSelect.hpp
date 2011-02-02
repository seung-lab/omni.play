#ifndef OM_BRUSH_SELECT_HPP
#define OM_BRUSH_SELECT_HPP

#include "utility/dataWrappers.h"
#include "view2d/omBrushSelectLineTask.hpp"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omBrushSize.hpp"
#include "view2d/omView2dManager.hpp"

class OmBrushSelect {
private:
	static int brushDiameterForMip(OmView2dState* state)
	{
		const float factor = om::pow2int(state->getMipLevel());
		const int brushDia = state->getBrushSize()->Diameter();
		return brushDia * factor;
	}

	static int getDepth(const DataCoord& coord, const ViewType viewType){
		return OmView2dConverters::GetViewTypeDepth(coord, viewType);
	}

	static boost::shared_ptr<OmBrushSelectCircle>
	makeCircle(OmView2dState* state, const DataCoord& coord)
	{
		SegmentationDataWrapper sdw(state->GetSegmentationID());
		OmSegmentation* segmentation = sdw.GetSegmentationPtr();

		const ViewType viewType = state->getViewType();
		const int brushSize = brushDiameterForMip(state);
		const int depth = getDepth(coord, viewType);

		return boost::make_shared<OmBrushSelectCircle>(segmentation, viewType,
													   brushSize, depth);
	}

public:
	static void SelectByClick(OmView2dState* state, const DataCoord& coord)
	{
		boost::shared_ptr<OmBrushSelectCircle> circle = makeCircle(state, coord);
		circle->SelectAndSendEvents(coord);
	}

	static void SelectByLine(OmView2dState* state,
							 const DataCoord& first,
							 const DataCoord& second)
	{
		boost::shared_ptr<OmBrushSelectCircle> circle = makeCircle(state, second);

		boost::shared_ptr<OmBrushSelectLineTask> task =
			boost::make_shared<OmBrushSelectLineTask>(circle, first, second);

		OmView2dManager::AddTaskBack(task);
	}
};

#endif

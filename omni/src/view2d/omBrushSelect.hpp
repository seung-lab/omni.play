#ifndef OM_BRUSH_SELECT_HPP
#define OM_BRUSH_SELECT_HPP

#include "utility/dataWrappers.h"
#include "view2d/omBrushSelectLine.hpp"
#include "view2d/omView2dState.hpp"
#include "viewGroup/omBrushSize.hpp"

class OmBrushSelect {
public:
	static void SelectByClick(OmView2dState* state, const DataCoord& gDC)
	{
		SegmentationDataWrapper sdw(state->GetSegmentationID());

		OmBrushSelectCircle s(sdw.GetSegmentationPtr(),
							  state->getViewType(),
							  state->getBrushSize()->Diameter(),
							  state->getSliceDepth());
		s.SelectAndSendEvents(gDC);
	}

	static void SelectByLine(OmView2dState* state,
							 const DataCoord& first,
							 const DataCoord& second)
	{
		SegmentationDataWrapper sdw(state->GetSegmentationID());

		OmBrushSelectLine s(sdw.GetSegmentationPtr(),
							state->getViewType(),
							state->getBrushSize()->Diameter(),
							state->getSliceDepth());

		s.SelectLine(first, second);
	}
};

#endif

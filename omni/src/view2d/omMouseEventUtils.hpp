#ifndef OM_MOUSE_EVENT_UTILS_HPP
#define OM_MOUSE_EVENT_UTILS_HPP

#include "view2d/omMouseZoom.hpp"

class OmView2d;

class OmMouseEventUtils{
public:
	static void PickToolAddToSelection(const SegmentDataWrapper& sdw,
					   const DataCoord& dataClickPoint,
					   OmView2d* v2d)
	{
		const OmSegID segID = sdw.GetVoxelValue(dataClickPoint);
		if (segID) {
			OmSegmentSelector sel(sdw.getSegmentationID(),
					      v2d,
					      "view2dpick" );
			sel.augmentSelectedSet( segID, true );
			sel.sendEvent();
		}
	}


};

#endif

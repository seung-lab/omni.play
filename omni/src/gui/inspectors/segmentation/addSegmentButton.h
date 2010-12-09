#ifndef ADD_SEGMENT_BUTTON_H
#define ADD_SEGMENT_BUTTON_H

#include "gui/widgets/omButton.hpp"
#include "gui/inspectors/segmentation/segInspector.h"
#include "utility/dataWrappers.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"

class AddSegmentButton : public OmButton<SegInspector> {
public:
	AddSegmentButton(SegInspector * d)
		: OmButton<SegInspector>( d,
								  "Add Segment",
								  "Add Segment",
								  false)
	{
	}

private:
	void doAction()
	{
		const SegmentationDataWrapper& sdw = mParent->GetSegmentationDataWrapper();

		OmSegment* newSeg = sdw.GetSegmentCache()->AddSegment();
		mParent->rebuildSegmentLists(newSeg->GetSegmentationID(),
									 newSeg->value());

		OmSegmentSelector sel(sdw, this, "addSegmentButton" );
		sel.selectJustThisSegment(newSeg->value(), true);
		sel.sendEvent();
	}
};

#endif

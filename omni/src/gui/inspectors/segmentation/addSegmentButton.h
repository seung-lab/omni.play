#ifndef ADD_SEGMENT_BUTTON_H
#define ADD_SEGMENT_BUTTON_H

#include "gui/widgets/omButton.h"
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
		SegmentationDataWrapper sdw = mParent->getSegmentationDataWrapper();
		OmSegment * newSeg = sdw.getSegmentCache()->AddSegment();
		mParent->rebuildSegmentLists(newSeg->getSegmentationID(), newSeg->value);

		OmSegmentSelector sel(sdw.getID(), this, "addSegmentButton" );
		sel.selectJustThisSegment(newSeg->value, true);
		sel.sendEvent();
	}
};

#endif

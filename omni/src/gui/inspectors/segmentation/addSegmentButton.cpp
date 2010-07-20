#include "gui/inspectors/segmentation/addSegmentButton.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"

AddSegmentButton::AddSegmentButton(SegInspector * d)
	: OmButton<SegInspector>( d, 
				 "Add Segment", 
				 "Add Segment", 
				 false)
{
}

void AddSegmentButton::doAction()
{
	SegmentationDataWrapper sdw = mParent->getSegmentationDataWrapper();
	OmSegment * newSeg = sdw.getSegmentCache()->AddSegment();
	mParent->rebuildSegmentLists(newSeg->getSegmentationID(), newSeg->getValue());

	OmSegmentSelector sel(sdw.getID(), this, "addSegmentButton" );
	sel.selectJustThisSegment(newSeg->getValue(), true);
	sel.sendEvent();
}

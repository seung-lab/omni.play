#include "gui/inspectors/segmentation/addSegmentButton.h"
#include "segment/omSegmentCache.h"

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
}

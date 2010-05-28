#include "segment/omSegmentSelector.h"
#include "project/omProject.h"
#include "volume/omSegmentation.h"
#include "segment/actions/segment/omSegmentSelectAction.h"

OmSegmentSelector::OmSegmentSelector( const OmId segmentationID, void * sender, const string & cmt )
	: mSegmentationID(segmentationID)
	, mSender(sender)
	, mComment(cmt)
	, mSegmentJustSelectedID(0)
{
}

void OmSegmentSelector::selectNoSegments()
{
	newlyUnselectedSegs = OmProject::GetSegmentation( mSegmentationID ).GetSelectedSegmentIds();
}

void OmSegmentSelector::selectJustThisSegment( const OmSegID segID, const bool isSelected )
{
	selectNoSegments();

	if(isSelected) {
		newlyUnselectedSegs.erase(segID);
		newlySelectedSegs.insert(segID);
		mSegmentJustSelectedID = segID;
	} else {
		newlyUnselectedSegs.insert(segID);
	}
}

void OmSegmentSelector::augmentSelectedSet( const OmSegID segID, const bool isSelected )
{
	if(isSelected) {
		newlySelectedSegs.insert(segID);
	} else {
		newlyUnselectedSegs.insert(segID);	
	}
}

void OmSegmentSelector::sendEvent()
{
	OmSegmentSelectAction * a = new OmSegmentSelectAction(mSegmentationID,
							      newlySelectedSegs, 
							      newlyUnselectedSegs,
							      mSegmentJustSelectedID, 
							      mSender,
							      mComment);
	a->Run();

	// don't delete--cleanup will be handled by OmAction (hopefully...)
}

#include "segment/omSegmentSelector.h"
#include "project/omProject.h"
#include "volume/omSegmentation.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/omSegmentCache.h"

OmSegmentSelector::OmSegmentSelector( const OmId segmentationID, void * sender, const string & cmt )
	: mSegmentation(&OmProject::GetSegmentation( segmentationID ))
	, mSender(sender)
	, mComment(cmt)
	, mSegmentJustSelectedID(0)
{
}

void OmSegmentSelector::selectNoSegments()
{
	newlyUnselectedSegs = mSegmentation->GetSelectedSegmentIds();
}

void OmSegmentSelector::selectJustThisSegment( const OmSegID segIDunknownLevel, const bool isSelected )
{
	selectNoSegments();

	const OmSegID segID = mSegmentation->GetSegmentCache()->findRootID( segIDunknownLevel );

	if(isSelected) {
		newlyUnselectedSegs.erase(segID);
		newlySelectedSegs.insert(segID);
		mSegmentJustSelectedID = segID;
	} else {
		newlyUnselectedSegs.insert(segID);
	}
}

void OmSegmentSelector::augmentSelectedSet( const OmSegID segIDunknownLevel, const bool isSelected )
{
	const OmSegID segID = mSegmentation->GetSegmentCache()->findRootID( segIDunknownLevel );

	if(isSelected) {
		newlySelectedSegs.insert(segID);
	} else {
		newlyUnselectedSegs.insert(segID);	
	}
}

void OmSegmentSelector::selectJustThisSegment_toggle( const OmSegID segIDunknownLevel )
{
	const OmSegID segID = mSegmentation->GetSegmentCache()->findRootID( segIDunknownLevel );
	const bool isSelected = mSegmentation->IsSegmentSelected( segID );
	selectJustThisSegment( segID, !isSelected );
}

void OmSegmentSelector::augmentSelectedSet_toggle( const OmSegID segIDunknownLevel )
{
	const OmSegID segID = mSegmentation->GetSegmentCache()->findRootID( segIDunknownLevel );
	const bool isSelected = mSegmentation->IsSegmentSelected( segID );
	augmentSelectedSet( segID, !isSelected );
}

void OmSegmentSelector::sendEvent()
{
	OmSegmentSelectAction * a = new OmSegmentSelectAction(mSegmentation->GetId(),
							      newlySelectedSegs, 
							      newlyUnselectedSegs,
							      mSegmentJustSelectedID, 
							      mSender,
							      mComment);
	a->Run();

	// don't delete--cleanup will be handled by OmAction (hopefully...)
}

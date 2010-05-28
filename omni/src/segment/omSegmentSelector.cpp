#include "segment/omSegmentSelector.h"
#include "project/omProject.h"
#include "volume/omSegmentation.h"
#include "segment/actions/segment/omSegmentSelectAction.h"

OmSegmentSelector::OmSegmentSelector( const OmId segmentationID, void * sender, const string & cmt )
	: mSender(sender)
	, mComment(cmt)
	, mSegmentJustSelectedID(0)
{
	mSegmentation = &OmProject::GetSegmentation( segmentationID );
}

void OmSegmentSelector::selectNoSegments()
{
	foreach( const OmSegID & segID, mSegmentation->GetSelectedSegmentIds() ){
		newlyUnselectedSegs.insert( segID );
	}
	mSegmentation->SetAllSegmentsSelected(false);
}

void OmSegmentSelector::selectJustThisSegment( const OmSegID segID, const bool isSelected )
{
	selectNoSegments();

	if(isSelected) {
		if( newlyUnselectedSegs.contains( segID ) ){
			newlyUnselectedSegs.erase( segID );
		}
		newlySelectedSegs.insert(segID);
		mSegmentJustSelectedID = segID;
	} else {
		newlyUnselectedSegs.insert(segID);
	}

	mSegmentation->SetSegmentSelected( segID, isSelected );
}

void OmSegmentSelector::augmentSelectedSet( const OmSegID segID, const bool isSelected )
{
	if(isSelected) {
		newlySelectedSegs.insert(segID);
	} else {
		newlyUnselectedSegs.insert(segID);	
	}

	mSegmentation->SetSegmentSelected( segID, isSelected);
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
}

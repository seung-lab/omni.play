#include "project/omProject.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"
#include "volume/omSegmentation.h"

OmSegmentSelector::OmSegmentSelector( const OmId segmentationID, void * sender, const string & cmt )
	: mSegmentation(&OmProject::GetSegmentation( segmentationID ))
	, mSegmentJustSelectedID(0)
	, mSender(sender)
	, mComment(cmt)
	, oldSelectedIDs( mSegmentation->GetSelectedSegmentIds() )
	, newSelectedIDs( oldSelectedIDs )
{
}

void OmSegmentSelector::selectNoSegments()
{
	newSelectedIDs.clear();
}

void OmSegmentSelector::selectJustThisSegment( const OmSegID segIDunknownLevel, const bool isSelected )
{
	selectNoSegments();

	const OmSegID segID = mSegmentation->GetSegmentCache()->findRootID( segIDunknownLevel );

	if( oldSelectedIDs.size() > 1 ){
		newSelectedIDs.insert( segID );
		mSegmentJustSelectedID = segID;
	} else {
		if( isSelected ){
			newSelectedIDs.insert( segID );
			mSegmentJustSelectedID = segID;
		}
	}
}

void OmSegmentSelector::augmentSelectedSet( const OmSegID segIDunknownLevel, const bool isSelected )
{
	const OmSegID segID = mSegmentation->GetSegmentCache()->findRootID( segIDunknownLevel );

	if(isSelected) {
		newSelectedIDs.insert(segID);
	} else {
		newSelectedIDs.erase(segID);	
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
							      newSelectedIDs, 
							      oldSelectedIDs,
							      mSegmentJustSelectedID, 
							      mSender,
							      mComment);
	a->Run();

	// don't delete--cleanup will be handled by OmAction (hopefully...)
}

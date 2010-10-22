#include "project/omProject.h"
#include "actions/omSegmentSelectAction.h"
#include "segment/actions/omSegmentEditor.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"
#include "volume/omSegmentation.h"

OmSegmentSelector::OmSegmentSelector( const OmID segmentationID, void * sender,
									  const std::string & cmt )
	: mSegmentation(&OmProject::GetSegmentation( segmentationID ))
	, mSegmentJustSelectedID(0)
	, mSender(sender)
	, mComment(cmt)
	, oldSelectedIDs( mSegmentation->GetSegmentCache()->GetSelectedSegmentIds() )
	, newSelectedIDs( oldSelectedIDs )
	, mAddToRecentList(true)
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
	if(!segID){
		return;
	}

	if( oldSelectedIDs.size() > 1 ){
		newSelectedIDs.insert( segID );
	} else {
		if( isSelected ){
			newSelectedIDs.insert( segID );
		}
	}

	setEditSelection(segID);
}

void OmSegmentSelector::setEditSelection(const OmSegID segID)
{
	OmSegmentEditor::SetEditSelection(mSegmentation->GetID(), segID);
	mSegmentJustSelectedID = segID;
}

void OmSegmentSelector::augmentSelectedSet( const OmSegID segIDunknownLevel, const bool isSelected )
{
	const OmSegID segID = mSegmentation->GetSegmentCache()->findRootID( segIDunknownLevel );

	if(!segID){
		return;
	}

	if(isSelected) {
		newSelectedIDs.insert(segID);
	} else {
		newSelectedIDs.erase(segID);
	}

	setEditSelection(segID);
}

void OmSegmentSelector::selectJustThisSegment_toggle( const OmSegID segIDunknownLevel )
{
	const OmSegID segID = mSegmentation->GetSegmentCache()->findRootID( segIDunknownLevel );
	if(!segID){
		return;
	}

	const bool isSelected = mSegmentation->GetSegmentCache()->IsSegmentSelected( segID );
	selectJustThisSegment( segID, !isSelected );
}

void OmSegmentSelector::augmentSelectedSet_toggle( const OmSegID segIDunknownLevel )
{
	const OmSegID segID = mSegmentation->GetSegmentCache()->findRootID( segIDunknownLevel );
	if(!segID){
		return;
	}

	const bool isSelected = mSegmentation->GetSegmentCache()->IsSegmentSelected( segID );
	augmentSelectedSet( segID, !isSelected );
}

bool OmSegmentSelector::sendEvent()
{
	if( oldSelectedIDs == newSelectedIDs ){
		return false;
	}

	OmSegmentSelectAction * a = new OmSegmentSelectAction(mSegmentation->GetID(),
														  newSelectedIDs,
														  oldSelectedIDs,
														  mSegmentJustSelectedID,
														  mSender,
														  mComment,
														  true,
														  mAddToRecentList);
	a->Run();
	// don't delete--cleanup will be handled by OmAction

	return true;
}

void OmSegmentSelector::setAddToRecentList(const bool shouldAdd)
{
	mAddToRecentList = shouldAdd;
}

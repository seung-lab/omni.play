#include "project/omProject.h"
#include "actions/omActions.hpp"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"
#include "volume/omSegmentation.h"

OmSegmentSelector::OmSegmentSelector(const SegmentationDataWrapper& sdw,
									  void* sender,
									  const std::string& cmt )
	: sdw_(sdw, 0)
	, segmentCache_(sdw_.GetSegmentCache())
	, mSender(sender)
	, mComment(cmt)
	, oldSelectedIDs(segmentCache_->GetSelectedSegmentIds())
	, newSelectedIDs(oldSelectedIDs)
	, mAddToRecentList(true)
{}

void OmSegmentSelector::selectNoSegments()
{
	newSelectedIDs.clear();
}

void OmSegmentSelector::selectJustThisSegment( const OmSegID segIDunknownLevel,
											   const bool isSelected )
{
	selectNoSegments();

	const OmSegID segID = segmentCache_->findRootID( segIDunknownLevel );
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

	setSelectedSegment(segID);
}

void OmSegmentSelector::setSelectedSegment(const OmSegID segID)
{
	sdw_.SetSegmentID(segID);
	OmSegmentSelected::Set(sdw_);
}

void OmSegmentSelector::augmentSelectedSet( const OmSegID segIDunknownLevel,
											const bool isSelected )
{
	const OmSegID segID = segmentCache_->findRootID( segIDunknownLevel );

	if(!segID){
		return;
	}

	if(isSelected) {
		newSelectedIDs.insert(segID);
	} else {
		newSelectedIDs.erase(segID);
	}

	setSelectedSegment(segID);
}

void OmSegmentSelector::selectJustThisSegment_toggle( const OmSegID segIDunknownLevel )
{
	const OmSegID segID = segmentCache_->findRootID( segIDunknownLevel );
	if(!segID){
		return;
	}

	const bool isSelected = segmentCache_->IsSegmentSelected( segID );
	selectJustThisSegment( segID, !isSelected );
}

void OmSegmentSelector::augmentSelectedSet_toggle( const OmSegID segIDunknownLevel )
{
	const OmSegID segID = segmentCache_->findRootID( segIDunknownLevel );
	if(!segID){
		return;
	}

	const bool isSelected = segmentCache_->IsSegmentSelected( segID );
	augmentSelectedSet( segID, !isSelected );
}

bool OmSegmentSelector::sendEvent()
{
	if( oldSelectedIDs == newSelectedIDs ){
		return false;
	}

	OmActions::SelectSegments(sdw_,
							  newSelectedIDs,
							  oldSelectedIDs,
							  mSender,
							  mComment,
							  true,
							  mAddToRecentList);
	return true;
}

void OmSegmentSelector::setAddToRecentList(const bool shouldAdd)
{
	mAddToRecentList = shouldAdd;
}

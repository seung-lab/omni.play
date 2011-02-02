#include "project/omProject.h"
#include "actions/omActions.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentSelector.h"
#include "volume/omSegmentation.h"

OmSegmentSelector::OmSegmentSelector(const SegmentationDataWrapper& sdw,
									  void* sender,
									  const std::string& cmt )
	: sdw_(sdw, 0)
	, segmentCache_(sdw_.SegmentCache())
	, mSender(sender)
	, mComment(cmt)
	, oldSelectedIDs(segmentCache_->GetSelectedSegmentIds())
	, newSelectedIDs(oldSelectedIDs)
	, autoCenter_(false)
	, shouldScroll_(true)
	, addToRecentList_(true)
	, augmentListOnly_(false)
{}

void OmSegmentSelector::selectNoSegments()
{
	newSelectedIDs.clear();
}

void OmSegmentSelector::selectJustThisSegment( const OmSegID segIDunknownLevel,
											   const bool isSelected)
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

void OmSegmentSelector::InsertSegments(const boost::unordered_set<OmSegID>& segIDs)
{
	FOR_EACH(iter, segIDs){
		newSelectedIDs.insert(segmentCache_->findRootID(*iter));
	}
}

void OmSegmentSelector::augmentSelectedSet( const OmSegID segIDunknownLevel,
											const bool isSelected)
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

void OmSegmentSelector::selectJustThisSegment_toggle(const OmSegID segIDunknownLevel)
{
	const OmSegID segID = segmentCache_->findRootID( segIDunknownLevel );
	if(!segID){
		return;
	}

	const bool isSelected = segmentCache_->IsSegmentSelected( segID );
	selectJustThisSegment( segID, !isSelected );
}

void OmSegmentSelector::augmentSelectedSet_toggle(const OmSegID segIDunknownLevel)
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
//		printf("not sending segment list\n");
		return false;
	}

//	std::cout << oldSelectedIDs << "\n";

	OmActions::SelectSegments(sdw_,
							  newSelectedIDs,
							  oldSelectedIDs,
							  mSender,
							  mComment,
							  shouldScroll_,
							  addToRecentList_,
							  autoCenter_,
							  augmentListOnly_);
	return true;
}

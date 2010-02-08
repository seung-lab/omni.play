
#include "omSegmentSelectAction.h"

#include "segment/omSegmentEditor.h"
#include "volume/omVolume.h"
#include "volume/omSegmentation.h"

#include "system/omEventManager.h"
#include "system/events/omSegmentEvent.h"
#include "utility/setUtilities.h"

#define DEBUG 0

#pragma mark -
#pragma mark OmSegmentSelectAction
/////////////////////////////////
///////
///////          OmSegmentSelectAction
///////

OmSegmentSelectAction::OmSegmentSelectAction(OmId segmentationId,
					     OmId segmentId,
					     const bool state, const OmId segmentJustSelected)
{
	OmIds segment_set, empty_set;
	segment_set.insert(segmentId);

	if (state) {
		Initialize(segmentationId, segment_set, empty_set, segmentJustSelected, NULL, string("") );
	} else {
		Initialize(segmentationId, empty_set, segment_set, segmentJustSelected, NULL, string("") );
	}
}

OmSegmentSelectAction::OmSegmentSelectAction(OmId segmentationId,
					     const OmIds & segmentIds,
					     const bool state, const OmId segmentJustSelected)
{
	OmIds empty_set;

	if (state) {
		Initialize(segmentationId, segmentIds, empty_set, segmentJustSelected, NULL, string("") );
	} else {
		Initialize(segmentationId, empty_set, segmentIds, segmentJustSelected, NULL, string("") );
	}
}

OmSegmentSelectAction::OmSegmentSelectAction(OmId segmentationId,
					     const OmIds & selectIds,
					     const OmIds & unselectIds, const OmId segmentJustSelected, 
					     void *sender, string comment )
{
	Initialize(segmentationId, selectIds, unselectIds, segmentJustSelected, sender, comment );
}

/*
 *	Initialize action data
 */
void
 OmSegmentSelectAction::Initialize(OmId segmentationId,
				   const OmIds & selectIds,
				   const OmIds & unselectIds, 
				   const OmId segmentJustSelected, 
				   void *sender, 
				   string comment )
{
	mSegmentJustSelectedID = segmentJustSelected;

	//store segmentation id
	mSegmentationId = segmentationId;

	//store modified seg ids
	mSelectIds = selectIds;
	mUnselectIds = unselectIds;

	mSender = sender;
	mComment = comment;

	//store old state of all changed segment ids
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(segmentationId);
	OmIds::iterator itr;
	for (itr = mSelectIds.begin(); itr != mSelectIds.end(); itr++) {
		mPrevSegmentStates[*itr] = r_segmentation.IsSegmentSelected(*itr);
	}
	for (itr = mUnselectIds.begin(); itr != mUnselectIds.end(); itr++) {
		mPrevSegmentStates[*itr] = r_segmentation.IsSegmentSelected(*itr);
	}
}

#pragma mark
#pragma mark Action Methods
/////////////////////////////////
///////          Action Methods

void OmSegmentSelectAction::Action()
{

	//get refs to volume and segmentation
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//set states
	OmIds::iterator itr;
	for (itr = mSelectIds.begin(); itr != mSelectIds.end(); itr++) {
		r_segmentation.SetSegmentSelected(*itr, true);
	}
	for (itr = mUnselectIds.begin(); itr != mUnselectIds.end(); itr++) {
		r_segmentation.SetSegmentSelected(*itr, false);
	}

	//send segment selection change event
	OmIds modified_segment_ids;
	setUnion < OmId > (mSelectIds, mUnselectIds, modified_segment_ids);
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
						     mSegmentationId,
						     modified_segment_ids, 
						     mSegmentJustSelectedID, 
						     mSender,
						     mComment));
}

void OmSegmentSelectAction::UndoAction()
{

	//get refs to volume and segmentation
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//for all segments in map, set old state of segment
	map < OmId, bool >::iterator itr;
	for (itr = mPrevSegmentStates.begin(); itr != mPrevSegmentStates.end(); itr++) {
		r_segmentation.SetSegmentSelected(itr->first, mPrevSegmentStates[itr->first]);
	}

	//send segment selection change event
	OmIds modified_segment_ids;
	setUnion < OmId > (mSelectIds, mUnselectIds, modified_segment_ids);
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
						     mSegmentationId, modified_segment_ids));
}

string OmSegmentSelectAction::Description()
{

	string plurlize;
	if (mPrevSegmentStates.size() > 1)
		plurlize = "s";

	if (mNewState)
		return string("Select Segment") + plurlize;
	else
		return string("Unselect Segment") + plurlize;
}

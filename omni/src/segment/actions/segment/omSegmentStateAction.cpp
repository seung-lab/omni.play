
#include "omSegmentStateAction.h"

#include "segment/omSegmentEditor.h"
#include "volume/omVolume.h"
#include "volume/omSegmentation.h"

#include "system/omEventManager.h"
#include "system/events/omSegmentEvent.h"

#define DEBUG 0

#pragma mark -
#pragma mark OmSegmentStateAction
/////////////////////////////////
///////
///////          OmSegmentStateAction
///////

OmSegmentStateAction::OmSegmentStateAction(OmId segmentationId, OmId segmentId, bool state)
{
	OmIds segment_set;
	segment_set.insert(segmentId);
	Initialize(segmentationId, segment_set, state);
}

OmSegmentStateAction::OmSegmentStateAction(OmId segmentationId, const OmIds & segmentIds, bool state)
{
	Initialize(segmentationId, segmentIds, state);
}

/*
 *	Initialize action data
 */
void
 OmSegmentStateAction::Initialize(OmId segmentationId, const OmIds & segmentIds, bool state)
{

	//store new state
	mNewState = state;

	//store segmentation id
	mSegmentationId = segmentationId;

	//store modified seg ids
	mSegmentIds = segmentIds;

	//store old state of all changed segment ids
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(segmentationId);
	OmIds::iterator itr;
	for (itr = segmentIds.begin(); itr != segmentIds.end(); itr++) {
		mPrevSegmentStates[*itr] = r_segmentation.IsSegmentEnabled(*itr);
	}

}

#pragma mark
#pragma mark Action Methods
/////////////////////////////////
///////          Action Methods

void OmSegmentStateAction::Action()
{

	//get refs to volume and segmentation
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//for all segments in map, set new state of segment
	map < OmId, bool >::iterator itr;
	for (itr = mPrevSegmentStates.begin(); itr != mPrevSegmentStates.end(); itr++) {
		r_segmentation.SetSegmentEnabled(itr->first, mNewState);
	}

	//send segment selection change event
	OmEventManager::
	    PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION, mSegmentationId, mSegmentIds));
}

void OmSegmentStateAction::UndoAction()
{

	//get refs to volume and segmentation
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//for all segments in map, set old state of segment
	map < OmId, bool >::iterator itr;
	for (itr = mPrevSegmentStates.begin(); itr != mPrevSegmentStates.end(); itr++) {
		r_segmentation.SetSegmentEnabled(itr->first, mPrevSegmentStates[itr->first]);
	}

	//send segment selection change event
	OmEventManager::
	    PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION, mSegmentationId, mSegmentIds));
}

string OmSegmentStateAction::Description()
{

	string plurlize;
	if (mPrevSegmentStates.size() > 1)
		plurlize = "s";

	if (mNewState)
		return string("Enabled Segment") + plurlize;
	else
		return string("Disabled Segment") + plurlize;
}

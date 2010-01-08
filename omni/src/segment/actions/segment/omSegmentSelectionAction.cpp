
#include "omSegmentSelectionAction.h"

#include "segment/omSegmentEditor.h"
#include "volume/omVolume.h"
#include "volume/omSegmentation.h"

#include "system/omEventManager.h"
#include "system/events/omSegmentEvent.h"


#define DEBUG 0



#pragma mark -
#pragma mark OmVoxelSelectionAction Class
/////////////////////////////////
///////
///////		 OmVoxelSelectionAction Class
///////

OmSegmentSelectionAction::OmSegmentSelectionAction(OmId segmentationId, OmId segmentId, 
												   bool state, bool append)
: mNewState(state), mAppend(append) {

	/*
	cout << "OmSegmentSelectionAction" << endl;
	cout << "WARNING: this method will be changed to:" << endl;
	cout << "OmSegmentSelectionAction(OmId segmentationId, OmIds segmentIds, bool state)" << endl;
	*/
	
	//store ids
	mSegmentationId = segmentationId;
	mSegmentId = segmentId;
	
	
	//get refs to volume and segmentation
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(segmentationId);

	
	//get prev segment state
	mPrevState = r_segmentation.IsSegmentSelected(segmentId);
	
	
	//if not appending then store prev selected
	if(!mAppend) {
		mPrevSelectedSegments = r_segmentation.GetSelectedSegmentIds();
	}
	
}





#pragma mark 
#pragma mark Action Methods
/////////////////////////////////
///////		 Action Methods


void 
OmSegmentSelectionAction::Action() {
	
	//get refs to volume and segmentation
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	//if not appending, then clear other selected segments
	if(!mAppend) r_segmentation.SetAllSegmentsSelected( false );
	
	//set new state of segment
	r_segmentation.SetSegmentSelected( mSegmentId, mNewState );
	
	//send segment selection change event
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION, mSegmentId));
}


void 
OmSegmentSelectionAction::UndoAction()	{ 
	//get refs to volume and segmentation
	OmSegmentation &r_segmentation = OmVolume::GetSegmentation(mSegmentationId);
	
	//set old state of segment
	r_segmentation.SetSegmentSelected( mSegmentId, mPrevState );
	
	//if not appending, 
	if(!mAppend) {
		//then restore all selected segments
		OmIds::iterator itr;
		for(itr = mPrevSelectedSegments.begin(); itr != mPrevSelectedSegments.end(); itr++) {
			r_segmentation.SetSegmentSelected( *itr, true );
		}
	}
	
	//send segment selection change event
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION, mSegmentId));
}


string 
OmSegmentSelectionAction::Description() {
	
	if(mNewState)
		return "Segment Selected";
	else
		return "Segment Unselected";
}
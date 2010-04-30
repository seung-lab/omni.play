
#include "omSegmentSelectAction.h"

#include "segment/omSegmentEditor.h"
#include "volume/omVolume.h"
#include "volume/omSegmentation.h"

#include "system/omEventManager.h"
#include "system/events/omSegmentEvent.h"
#include "utility/setUtilities.h"

#define DEBUG 0

/////////////////////////////////
///////
///////          OmSegmentSelectAction
///////

OmSegmentSelectAction::OmSegmentSelectAction(OmId segmentationId,
					     OmId segmentId,
					     const bool state, 
					     const OmId segmentJustSelected)
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
					     const bool state, 
					     const OmId segmentJustSelected)
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
					     const OmIds & unselectIds, 
					     const OmId segmentJustSelected, 
					     void *sender, 
					     string comment )
{
	Initialize(segmentationId, selectIds, unselectIds, segmentJustSelected, sender, comment );
}

void OmSegmentSelectAction::Initialize(OmId segmentationId,
				       const OmIds & selectIds,
				       const OmIds & unselectIds, 
				       const OmId segmentJustSelected, 
				       void *sender, 
				       string comment )
{
	mSegmentJustSelectedID = segmentJustSelected;
	mSegmentationId = segmentationId;
	mSelectIds = selectIds;
	mUnselectIds = unselectIds;

	mSender = sender;
	mComment = comment;
}

/////////////////////////////////
///////          Action Methods
void OmSegmentSelectAction::Action()
{
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mSegmentationId);

	foreach( OmId segID, mSelectIds ){
		r_segmentation.SetSegmentSelected( segID, true);
	}
	foreach( OmId segID, mUnselectIds ){
		r_segmentation.SetSegmentSelected( segID, false);
	}

	//send segment selection change event
	OmIds modified_segment_ids = mSelectIds;
	modified_segment_ids.unite( mUnselectIds );
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
						     mSegmentationId,
						     modified_segment_ids, 
						     mSegmentJustSelectedID, 
						     mSender,
						     mComment));
}

void OmSegmentSelectAction::UndoAction()
{
}

string OmSegmentSelectAction::Description()
{
	return string("");
}

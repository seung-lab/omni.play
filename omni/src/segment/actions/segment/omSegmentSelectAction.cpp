#include "project/omProject.h"
#include "omSegmentSelectAction.h"

#include "segment/omSegmentEditor.h"
#include "volume/omVolume.h"
#include "volume/omSegmentation.h"

#include "system/omEventManager.h"
#include "system/events/omSegmentEvent.h"
#include "utility/setUtilities.h"
#include "utility/dataWrappers.h"

/////////////////////////////////
///////          OmSegmentSelectAction

OmSegmentSelectAction::OmSegmentSelectAction(const OmId segmentationId,
					     const OmSegIDs & selectIds,
					     const OmSegIDs & unselectIds, 
					     const OmId segmentJustSelected, 
					     void * sender, 
					     string comment )
{
	mSegmentJustSelectedID = segmentJustSelected;
	mSegmentationId = segmentationId;
	mSelectIds = selectIds;
	mUnselectIds = unselectIds;

	mSender = sender;
	mComment = comment;

	OmSegIDs::const_iterator iter;
	for( iter =  mSelectIds.begin(); iter != mSelectIds.end(); ++iter ){
		modifiedSegIDs.insert(*iter);
	}
	for( iter = mUnselectIds.begin(); iter != mUnselectIds.end(); ++iter ){
		modifiedSegIDs.insert(*iter);
	}
}

/////////////////////////////////
///////          Action Methods
void OmSegmentSelectAction::Action()
{
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
						     mSegmentationId,
						     modifiedSegIDs,
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

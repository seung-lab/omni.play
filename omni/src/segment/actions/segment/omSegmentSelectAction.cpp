#include "project/omProject.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "system/events/omSegmentEvent.h"
#include "system/omEventManager.h"
#include "volume/omSegmentation.h"

/////////////////////////////////
///////          OmSegmentSelectAction

OmSegmentSelectAction::OmSegmentSelectAction(const OmId segmentationId,
					     const OmSegIDs & selectIds,
					     const OmSegIDs & unselectIds, 
					     const OmId segmentJustSelected, 
					     void * sender, 
					     const string & comment )
	: mSegmentationId(segmentationId)
	, mSelectIds(selectIds)
	, mUnselectIds(unselectIds)
	, mSegmentJustSelectedID(segmentJustSelected)
	, mSender(sender)
	, mComment(comment)
{
	OmSegIDs::const_iterator iter;
	for( iter =  mSelectIds.begin(); iter != mSelectIds.end(); ++iter ){
		mModifiedSegIDs.insert(*iter);
	}
	for( iter = mUnselectIds.begin(); iter != mUnselectIds.end(); ++iter ){
		mModifiedSegIDs.insert(*iter);
	}
}

/////////////////////////////////
///////          Action Methods

void OmSegmentSelectAction::Action()
{
	OmSegmentation & mSegmentation = OmProject::GetSegmentation( mSegmentationId );

	mSegmentation.UpdateSegmentSelection( mSelectIds, true );
	mSegmentation.UpdateSegmentSelection( mUnselectIds, false );

	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
						     mSegmentationId,
						     mModifiedSegIDs,
						     mSegmentJustSelectedID, 
						     mSender,
						     mComment));
}

void OmSegmentSelectAction::UndoAction()
{
	OmSegmentation & mSegmentation = OmProject::GetSegmentation( mSegmentationId );

	mSegmentation.UpdateSegmentSelection( mSelectIds, false );
	mSegmentation.UpdateSegmentSelection( mUnselectIds, true );

	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
						     mSegmentationId,
						     mModifiedSegIDs,
						     mSegmentJustSelectedID, 
						     mSender,
						     mComment));
}

string OmSegmentSelectAction::Description()
{
	return string("");
}

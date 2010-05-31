#include "project/omProject.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "system/events/omSegmentEvent.h"
#include "system/omEventManager.h"
#include "volume/omSegmentation.h"

/////////////////////////////////
///////          OmSegmentSelectAction

OmSegmentSelectAction::OmSegmentSelectAction(const OmId segmentationId,
					     const OmSegIDsSet & newSelectedIdSet,
					     const OmSegIDsSet & oldSelectedIdSet, 
					     const OmId segmentJustSelected, 
					     void * sender, 
					     const string & comment )
	: mSegmentationId(segmentationId)
	, mNewSelectedIdSet(newSelectedIdSet)
	, mOldSelectedIdSet(oldSelectedIdSet)
	, mSegmentJustSelectedID(segmentJustSelected)
	, mSender(sender)
	, mComment(comment)
{
}

/////////////////////////////////
///////          Action Methods

void OmSegmentSelectAction::Action()
{
	OmSegmentation & mSegmentation = OmProject::GetSegmentation( mSegmentationId );

	mSegmentation.UpdateSegmentSelection( mNewSelectedIdSet );

	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
						     mSegmentationId,
						     mSegmentJustSelectedID, 
						     mSender,
						     mComment));
}

void OmSegmentSelectAction::UndoAction()
{
	OmSegmentation & mSegmentation = OmProject::GetSegmentation( mSegmentationId );

	mSegmentation.UpdateSegmentSelection( mOldSelectedIdSet );

	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
						     mSegmentationId,
						     mSegmentJustSelectedID, 
						     mSender,
						     mComment));
}

string OmSegmentSelectAction::Description()
{
	return string("Yay! We did a selection!!");
}

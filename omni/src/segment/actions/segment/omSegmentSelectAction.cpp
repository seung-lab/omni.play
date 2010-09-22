#include "datalayer/fs/omActionLoggerFS.h"
#include "project/omProject.h"
#include "segment/actions/segment/omSegmentSelectAction.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "system/events/omViewEvent.h"
#include "system/omEventManager.h"
#include "volume/omSegmentation.h"

/////////////////////////////////
///////          OmSegmentSelectAction

OmSegmentSelectAction::OmSegmentSelectAction(const OmId segmentationId,
					     const OmSegIDsSet & newSelectedIdSet,
					     const OmSegIDsSet & oldSelectedIdSet,
					     const OmId segmentJustSelected,
					     void * sender,
					     const std::string& comment,
					     const bool doScroll,
					     const bool addToRecentList)
	: mSegmentationId(segmentationId)
	, mNewSelectedIdSet(newSelectedIdSet)
	, mOldSelectedIdSet(oldSelectedIdSet)
	, mSegmentJustSelectedID(segmentJustSelected)
	, mSender(sender)
	, mComment(comment)
	, mDoScroll(doScroll)
	, mAddToRecentList(addToRecentList)
{
}

/////////////////////////////////
///////          Action Methods

void OmSegmentSelectAction::Action()
{
	OmProject::GetSegmentation( mSegmentationId ).
		GetSegmentCache()->
		UpdateSegmentSelection( mNewSelectedIdSet, mAddToRecentList );

	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
						     mSegmentationId,
						     mSegmentJustSelectedID,
						     mSender,
						     mComment,
						     mDoScroll));
}

void OmSegmentSelectAction::UndoAction()
{
	OmProject::GetSegmentation( mSegmentationId ).
		GetSegmentCache()->
		UpdateSegmentSelection( mOldSelectedIdSet, mAddToRecentList );

	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION,
						     mSegmentationId,
						     mSegmentJustSelectedID,
						     mSender,
						     mComment,
						     mDoScroll));
}

std::string OmSegmentSelectAction::Description()
{
        QString lineItem = QString("Selected: ");
        foreach(const OmSegID segId, mNewSelectedIdSet){
                lineItem += QString("seg %1 + ").arg(segId);
        }

        return lineItem.toStdString();
}

void OmSegmentSelectAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(this, comment);
}

#include "datalayer/fs/omActionLoggerFS.h"
#include "omSegmentValidateAction.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "system/omEventManager.h"
#include "system/cache/omCacheManager.h"
#include "utility/setUtilities.h"
#include "volume/omSegmentation.h"

/////////////////////////////////
///////
///////          OmSegmentValidateAction
///////
OmSegmentValidateAction::OmSegmentValidateAction( const OmId segmentationId,
					  const OmSegIDsSet & selectedSegmentIds,
					  const bool create)
	: mSegmentationId( segmentationId )
	, mCreate(create)
	, mSelectedSegmentIds( selectedSegmentIds )
{
	SetUndoable(true);
}

/////////////////////////////////
///////          Action Methods
void OmSegmentValidateAction::Action()
{
	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	foreach(OmSegID id, mSelectedSegmentIds) {
        	OmSegment * segment = seg.GetSegmentCache()->GetSegment(id);
		segment->SetImmutable(mCreate);
        	seg.GetSegmentCache()->setAsValidated(segment, mCreate);
		printf("set %d as valid=%d\n", id, mCreate);
	}
	OmCacheManager::Freshen(true);
}

void OmSegmentValidateAction::UndoAction()
{
	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	foreach(OmSegID id, mSelectedSegmentIds) {
        	OmSegment * segment = seg.GetSegmentCache()->GetSegment(id);
		segment->SetImmutable(!mCreate);
        	seg.GetSegmentCache()->setAsValidated(segment, !mCreate);
	}
	OmCacheManager::Freshen(true);
}

string OmSegmentValidateAction::Description()
{
	QString lineItem;
	if(mCreate) {
		lineItem = QString("Validated: ");
	} else {
		lineItem = QString("Invalidated: ");
	}
	int count = 0;
	foreach( const OmId segId, mSelectedSegmentIds){
		lineItem += QString("seg %1 + ").arg(segId);
		if(count > 10) break;
		count++;
	}

	return lineItem.toStdString();
}

void OmSegmentValidateAction::save(const string & comment)
{
	OmActionLoggerFS::save(this, comment);
}

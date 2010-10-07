#include "datalayer/fs/omActionLoggerFS.h"
#include "omSegmentValidateAction.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "system/omEventManager.h"
#include "system/cache/omCacheManager.h"
#include "utility/setUtilities.h"
#include "volume/omSegmentation.h"

OmSegmentValidateAction::OmSegmentValidateAction( const OmId segmentationId,
												  const OmSegIDsSet & selectedSegmentIds,
												  const bool valid)
	: mSegmentationId( segmentationId )
	, valid_(valid)
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
		printf("set seg %d as valid: %d\n", id, valid_);
		segment->SetImmutable(valid_);
		seg.GetSegmentCache()->setAsValidated(segment, valid_);
	}
	OmCacheManager::Freshen(true);
}

void OmSegmentValidateAction::UndoAction()
{
	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	foreach(OmSegID id, mSelectedSegmentIds) {
		OmSegment * segment = seg.GetSegmentCache()->GetSegment(id);
		printf("set seg %d as valid: %d\n", id, valid_);
		segment->SetImmutable(!valid_);
		seg.GetSegmentCache()->setAsValidated(segment, !valid_);
	}
	OmCacheManager::Freshen(true);
}

std::string OmSegmentValidateAction::Description()
{
	QString lineItem;
	if(valid_) {
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

void OmSegmentValidateAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(this, comment);
}

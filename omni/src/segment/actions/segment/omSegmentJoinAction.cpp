#include "datalayer/fs/omActionLoggerFS.h"
#include "omSegmentJoinAction.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "system/omEventManager.h"
#include "utility/dataWrappers.h"
#include "utility/setUtilities.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

/////////////////////////////////
///////
///////          OmSegmentJoinAction
///////
OmSegmentJoinAction::OmSegmentJoinAction( const OmId segmentationId, 
					  const OmSegIDsSet & selectedSegmentIds)
	: mSegmentationId( segmentationId )
	, mSelectedSegmentIds( selectedSegmentIds )
{
	SetUndoable(true);
}

/////////////////////////////////
///////          Action Methods
void OmSegmentJoinAction::Action()
{
	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	seg.GetSegmentCache()->JoinTheseSegments(mSelectedSegmentIds);
}

void OmSegmentJoinAction::UndoAction()
{
	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	seg.GetSegmentCache()->UnJoinTheseSegments(mSelectedSegmentIds);	
}

string OmSegmentJoinAction::Description()
{
	QString lineItem = QString("Joined: ");
	foreach( const OmId segId, mSelectedSegmentIds){
		lineItem += QString("seg %1 + ").arg(segId);
	}

	return lineItem.toStdString();
}

void OmSegmentJoinAction::save(const string & comment)
{
	OmActionLoggerFS::save(this, comment);
}

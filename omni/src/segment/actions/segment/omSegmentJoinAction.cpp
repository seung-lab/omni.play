#include "project/omProject.h"
#include "omSegmentJoinAction.h"

#include "segment/omSegmentEditor.h"
#include "volume/omVolume.h"
#include "volume/omSegmentation.h"

#include "system/omEventManager.h"
#include "system/events/omSegmentEvent.h"
#include "utility/setUtilities.h"
#include "utility/dataWrappers.h"

/////////////////////////////////
///////
///////          OmSegmentJoinAction
///////
OmSegmentJoinAction::OmSegmentJoinAction(OmId segmentationId, OmIds selectedSegmentIds)
{
	mSegmentationId = segmentationId;
	mSelectedSegmentIds = selectedSegmentIds;
	SetUndoable(true);
}


/////////////////////////////////
///////          Action Methods
void OmSegmentJoinAction::Action()
{
	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	seg.JoinTheseSegments(mSelectedSegmentIds);
}

void OmSegmentJoinAction::UndoAction()
{
	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	seg.UnJoinTheseSegments(mSelectedSegmentIds);	
}


string OmSegmentJoinAction::Description()
{
	QString lineItem = QString("Joined: ");
	foreach( const OmId segId, mSelectedSegmentIds){
		lineItem = lineItem + QString("seg %1 + ").arg(segId);
	}
	return lineItem.toStdString();
}

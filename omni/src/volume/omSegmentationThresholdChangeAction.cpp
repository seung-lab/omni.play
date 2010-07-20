#include "datalayer/fs/omActionLoggerFS.h"
#include "omSegmentationThresholdChangeAction.h"
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
///////          OmSegmentationThresholdChangeAction
///////
OmSegmentationThresholdChangeAction::OmSegmentationThresholdChangeAction( const OmId segmentationId, 
					  const float threshold)
	: mSegmentationId( segmentationId )
	, mThreshold( threshold )
{
	SetUndoable(true);
}

/////////////////////////////////
///////          Action Methods
void OmSegmentationThresholdChangeAction::Action()
{
	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	mOldThreshold = seg.GetDendThreshold();
	seg.SetDendThreshold(mThreshold);
}

void OmSegmentationThresholdChangeAction::UndoAction()
{
	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	seg.SetDendThreshold(mOldThreshold);
}

string OmSegmentationThresholdChangeAction::Description()
{
	QString lineItem = QString("Threshold: %1").arg(mThreshold);
	return lineItem.toStdString();
}

void OmSegmentationThresholdChangeAction::save(const string & comment)
{
	OmActionLoggerFS().save(this, comment);
}

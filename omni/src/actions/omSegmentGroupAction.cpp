#include "datalayer/fs/omActionLoggerFS.h"
#include "omSegmentGroupAction.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/omGroups.h"
#include "volume/omSegmentation.h"

/////////////////////////////////
///////
///////          OmSegmentGroupAction
///////
OmSegmentGroupAction::OmSegmentGroupAction( const OmID segmentationId,
					  const OmSegIDsSet & selectedSegmentIds,
					  const OmGroupName name, const bool create)
	: mSegmentationId( segmentationId )
	, mName(name)
	, mCreate(create)
	, mSelectedSegmentIds( selectedSegmentIds )
{
	SetUndoable(true);
}

/////////////////////////////////
///////          Action Methods
void OmSegmentGroupAction::Action()
{
	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	if(mCreate) {
        	seg.GetGroups()->SetGroup(mSelectedSegmentIds, mName);
	} else {
        	seg.GetGroups()->UnsetGroup(mSelectedSegmentIds, mName);
	}
}

void OmSegmentGroupAction::UndoAction()
{
	OmSegmentation & seg = OmProject::GetSegmentation(mSegmentationId);
	if(!mCreate) {
        	seg.GetGroups()->SetGroup(mSelectedSegmentIds, mName);
	} else {
        	seg.GetGroups()->UnsetGroup(mSelectedSegmentIds, mName);
	}
}

std::string OmSegmentGroupAction::Description()
{
	QString lineItem = QString("Grouped: ");
	foreach( const OmID segId, mSelectedSegmentIds){
		lineItem += QString("seg %1 + ").arg(segId);
	}

	return lineItem.toStdString();
}

void OmSegmentGroupAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(this, comment);
}

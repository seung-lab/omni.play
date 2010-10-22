#include "datalayer/fs/omActionLoggerFS.h"
#include "project/omProject.h"
#include "actions/omSegmentSelectAction.h"
#include "utility/dataWrappers.h"
#include "system/omEvents.h"

OmSegmentSelectAction::OmSegmentSelectAction(const OmID segmentationId,
											 const OmSegIDsSet & newSelectedIdSet,
											 const OmSegIDsSet & oldSelectedIdSet,
											 const OmID segmentJustSelected,
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
{}

void OmSegmentSelectAction::Action()
{
	SegmentationDataWrapper sdw(mSegmentationId);
	sdw.getSegmentCache()->UpdateSegmentSelection(mNewSelectedIdSet,
												  mAddToRecentList);

	OmEvents::SegmentModified(mSegmentationId,
							  mSegmentJustSelectedID,
							  mSender,
							  mComment,
							  mDoScroll);
}

void OmSegmentSelectAction::UndoAction()
{
	SegmentationDataWrapper sdw(mSegmentationId);
	sdw.getSegmentCache()->UpdateSegmentSelection(mOldSelectedIdSet,
												  mAddToRecentList);

	OmEvents::SegmentModified(mSegmentationId,
							  mSegmentJustSelectedID,
							  mSender,
							  mComment,
							  mDoScroll);
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

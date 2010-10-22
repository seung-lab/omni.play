#include "datalayer/fs/omActionLoggerFS.h"
#include "actions/omSegmentUncertainAction.h"
#include "system/cache/omCacheManager.h"
#include "segment/omSegmentUncertain.hpp"

void OmSegmentUncertainAction::SetUncertain(const SegmentDataWrapper& sdw,
											const bool uncertain)
{
	OmSegIDsSet set;
	set.insert(sdw.FindRootID());

	OmSegmentation & seg = sdw.getSegmentation();
	boost::shared_ptr<std::set<OmSegment*> > children =
		seg.GetAllChildrenSegments(set);

	(new OmSegmentUncertainAction(seg.GetID(), children, uncertain))->Run();
}

void OmSegmentUncertainAction::SetUncertain(const SegmentationDataWrapper& sdw,
											const bool uncertain)
{
	OmSegmentation & seg = sdw.getSegmentation();

	boost::shared_ptr<std::set<OmSegment*> > children =
		seg.GetAllChildrenSegments(seg.GetSegmentCache()->GetSelectedSegmentIds());

	(new OmSegmentUncertainAction(seg.GetID(), children, uncertain))->Run();
}

OmSegmentUncertainAction::OmSegmentUncertainAction(const OmID segmentationId,
												   boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
												   const bool uncertain)
	: mSegmentationId( segmentationId )
	, uncertain_(uncertain)
	, selectedSegments_(selectedSegments)
{
	SetUndoable(true);
}

/////////////////////////////////
///////          Action Methods
void OmSegmentUncertainAction::Action()
{
	OmSegmentUncertain::SetAsUncertain(SegmentationDataWrapper(mSegmentationId),
									   selectedSegments_,
									   uncertain_);
	OmCacheManager::TouchFresheness();
}

void OmSegmentUncertainAction::UndoAction()
{
	OmSegmentUncertain::SetAsUncertain(SegmentationDataWrapper(mSegmentationId),
									   selectedSegments_,
									   !uncertain_);
	OmCacheManager::TouchFresheness();
}

std::string OmSegmentUncertainAction::Description()
{
	QString lineItem;
	if(uncertain_) {
		lineItem = QString("Uncertain: ");
	} else {
		lineItem = QString("Not uncertain: ");
	}

	int count = 0;
	FOR_EACH(iter, *selectedSegments_){
		lineItem += QString("seg %1 + ").arg((*iter)->value());
		if(count > 10) break;
		count++;
	}

	return lineItem.toStdString();
}

void OmSegmentUncertainAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(this, comment);
}

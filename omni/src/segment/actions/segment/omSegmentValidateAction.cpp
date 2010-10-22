#include "datalayer/fs/omActionLoggerFS.h"
#include "segment/actions/segment/omSegmentValidateAction.h"
#include "segment/omSegmentValidation.hpp"
#include "system/cache/omCacheManager.h"

void OmSegmentValidateAction::Validate(const SegmentDataWrapper& sdw,
									   const bool valid)
{
	OmSegIDsSet set;
	set.insert(sdw.getSegmentCache()->findRootID(sdw.getID()));

	OmSegmentation & seg = sdw.getSegmentation();
	boost::shared_ptr<std::set<OmSegment*> > children =
		seg.GetAllChildrenSegments(set);

	(new OmSegmentValidateAction(seg.GetID(), children, valid))->Run();
}

void OmSegmentValidateAction::Validate(const SegmentationDataWrapper& sdw,
									   const bool valid)
{
	OmSegmentation & seg = sdw.getSegmentation();

	boost::shared_ptr<std::set<OmSegment*> > children =
		seg.GetAllChildrenSegments(seg.GetSegmentCache()->GetSelectedSegmentIds());

	(new OmSegmentValidateAction(seg.GetID(), children, valid))->Run();
}

OmSegmentValidateAction::OmSegmentValidateAction(const OmID segmentationId,
												 boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
												 const bool valid)
	: mSegmentationId( segmentationId )
	, valid_(valid)
	, selectedSegments_(selectedSegments)
{
	SetUndoable(true);
}

/////////////////////////////////
///////          Action Methods
void OmSegmentValidateAction::Action()
{
	OmSegmentValidation::SetAsValidated(SegmentationDataWrapper(mSegmentationId),
										selectedSegments_,
										valid_);
	OmCacheManager::TouchFresheness();
}

void OmSegmentValidateAction::UndoAction()
{
	OmSegmentValidation::SetAsValidated(SegmentationDataWrapper(mSegmentationId),
										selectedSegments_,
										!valid_);
	OmCacheManager::TouchFresheness();
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
	FOR_EACH(iter, *selectedSegments_){
		lineItem += QString("seg %1 + ").arg((*iter)->value());
		if(count > 10) break;
		count++;
	}

	return lineItem.toStdString();
}

void OmSegmentValidateAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(this, comment);
}

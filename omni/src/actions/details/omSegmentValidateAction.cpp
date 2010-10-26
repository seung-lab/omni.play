#include "actions/io/omActionLoggerFS.h"
#include "actions/details/omSegmentValidateAction.h"
#include "actions/details/omSegmentValidateActionImpl.hpp"

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
	: impl_(boost::make_shared<OmSegmentValidateActionImpl>(segmentationId,
															selectedSegments,
															valid))
{
	SetUndoable(true);
}

void OmSegmentValidateAction::Action()
{
	impl_->Execute();
}

void OmSegmentValidateAction::UndoAction()
{
	impl_->Undo();
}

std::string OmSegmentValidateAction::Description()
{
	return impl_->Description();
}

void OmSegmentValidateAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(impl_, comment);
}

#include "actions/io/omActionLoggerFS.h"
#include "actions/details/omSegmentValidateAction.h"
#include "actions/details/omSegmentValidateActionImpl.hpp"
#include "segment/omSegmentUtils.hpp"

void OmSegmentValidateAction::Validate(const SegmentDataWrapper& sdw,
									   const om::SetValid validEnum)
{
	bool valid = false;
	if(om::SET_VALID == validEnum){
		valid = true;
	}

	OmSegIDsSet set;
	set.insert(sdw.FindRootID());

	boost::shared_ptr<std::set<OmSegment*> > children =
		OmSegmentUtils::GetAllChildrenSegments(sdw.GetSegmentCache(), set);

	(new OmSegmentValidateAction(sdw.getID(), children, valid))->Run();
}

void OmSegmentValidateAction::Validate(const SegmentationDataWrapper& sdw,
									   const om::SetValid validEnum)
{
	bool valid = false;
	if(om::SET_VALID == validEnum){
		valid = true;
	}

	OmSegmentCache* segCache = sdw.GetSegmentCache();
	boost::shared_ptr<std::set<OmSegment*> > children =
		OmSegmentUtils::GetAllChildrenSegments(segCache,
											   segCache->GetSelectedSegmentIds());

	(new OmSegmentValidateAction(sdw.getID(), children, valid))->Run();
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

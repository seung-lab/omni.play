#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentJoinAction.h"
#include "actions/details/omSegmentJoinActionImpl.hpp"

OmSegmentJoinAction::OmSegmentJoinAction(const OmID segmentationId,
										 const OmSegIDsSet& selectedSegmentIds)
	: impl_(boost::make_shared<OmSegmentJoinActionImpl>(segmentationId,
														selectedSegmentIds))
{
	SetUndoable(true);
}

void OmSegmentJoinAction::Action()
{
	impl_->Execute();
	SetDescription();
}

void OmSegmentJoinAction::UndoAction()
{
	impl_->Undo();
	SetDescription();
}

std::string OmSegmentJoinAction::Description()
{
	return impl_->Description();
}

void OmSegmentJoinAction::save(const std::string& comment)
{
	OmActionLogger::save(impl_, comment);
}

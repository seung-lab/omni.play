#include "datalayer/fs/omActionLoggerFS.h"
#include "actions/omSegmentJoinAction.h"
#include "actions/omSegmentJoinActionImpl.hpp"

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
}

void OmSegmentJoinAction::UndoAction()
{
	impl_->Undo();
}

std::string OmSegmentJoinAction::Description()
{
	impl_->Description();
}

void OmSegmentJoinAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(impl_, comment);
}

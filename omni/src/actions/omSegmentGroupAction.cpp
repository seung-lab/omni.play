#include "datalayer/fs/omActionLoggerFS.h"
#include "actions/omSegmentGroupAction.h"
#include "actions/omSegmentGroupActionImpl.hpp"

OmSegmentGroupAction::OmSegmentGroupAction(const OmID segmentationId,
										   const OmSegIDsSet& selectedSegmentIds,
										   const OmGroupName name,
										   const bool create)
	: impl_(boost::make_shared<OmSegmentGroupActionImpl>(segmentationId,
														 selectedSegmentIds,
														 name,
														 create))
{
	SetUndoable(true);
}

void OmSegmentGroupAction::Action()
{
	impl_->Execute();
}

void OmSegmentGroupAction::UndoAction()
{
	impl_->Undo();
}

std::string OmSegmentGroupAction::Description()
{
	return impl_->Description();
}

void OmSegmentGroupAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(impl_, comment);
}

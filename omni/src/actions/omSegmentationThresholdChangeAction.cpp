#include "datalayer/fs/omActionLoggerFS.h"
#include "actions/omSegmentationThresholdChangeAction.h"
#include "actions/omSegmentationThresholdChangeActionImpl.hpp"

OmSegmentationThresholdChangeAction::OmSegmentationThresholdChangeAction( const OmID segmentationId,
																		  const float threshold)
	: impl_(boost::make_shared<OmSegmentationThresholdChangeActionImpl>(segmentationId,
																		threshold))
{
	SetUndoable(true);
}

void OmSegmentationThresholdChangeAction::Action()
{
	impl_->Execute();
}

void OmSegmentationThresholdChangeAction::UndoAction()
{
	impl_->Undo();
}

std::string OmSegmentationThresholdChangeAction::Description()
{
	return impl_->Description();
}

void OmSegmentationThresholdChangeAction::save(const std::string & comment)
{
	OmActionLoggerFS::save(impl_, comment);
}

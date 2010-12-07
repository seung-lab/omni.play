#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentationThresholdChangeAction.h"
#include "actions/details/omSegmentationThresholdChangeActionImpl.hpp"

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
	OmActionLogger::save(impl_, comment);
}

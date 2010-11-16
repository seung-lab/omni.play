#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentSelectAction.h"
#include "actions/details/omSegmentSelectActionImpl.hpp"

OmSegmentSelectAction::OmSegmentSelectAction(const OmID segmentationId,
											 const OmSegIDsSet & newSelectedIdSet,
											 const OmSegIDsSet & oldSelectedIdSet,
											 const OmID segmentJustSelected,
											 void * sender,
											 const std::string& comment,
											 const bool doScroll,
											 const bool addToRecentList)
	: impl_(boost::make_shared<OmSegmentSelectActionImpl>(segmentationId,
														 newSelectedIdSet,
														 oldSelectedIdSet,
														 segmentJustSelected,
														 sender,
														 comment,
														 doScroll,
														 addToRecentList))
{}

void OmSegmentSelectAction::Action()
{
	impl_->Execute();
}

void OmSegmentSelectAction::UndoAction()
{
	impl_->Undo();
}

std::string OmSegmentSelectAction::Description()
{
	return impl_->Description();
}

void OmSegmentSelectAction::save(const std::string& comment)
{
	OmActionLogger::save(impl_, comment);
}

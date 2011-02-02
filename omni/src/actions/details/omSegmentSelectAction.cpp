#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentSelectAction.h"
#include "actions/details/omSegmentSelectActionImpl.hpp"

OmSegmentSelectAction::OmSegmentSelectAction(const SegmentDataWrapper& sdw,
											 const OmSegIDsSet & newSelectedIdSet,
											 const OmSegIDsSet & oldSelectedIdSet,
											 void * sender,
											 const std::string& comment,
											 const bool doScroll,
											 const bool addToRecentList,
											 const bool center,
											 const bool augmentListOnly)
	: impl_(boost::make_shared<OmSegmentSelectActionImpl>(sdw,
														  newSelectedIdSet,
														  oldSelectedIdSet,
														  sender,
														  comment,
														  doScroll,
														  addToRecentList,
														  center,
														  augmentListOnly))
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

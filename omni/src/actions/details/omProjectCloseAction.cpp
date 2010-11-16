#include "actions/io/omActionLogger.hpp"
#include "omProjectCloseAction.h"
#include "actions/details/omProjectCloseActionImpl.hpp"

OmProjectCloseAction::OmProjectCloseAction()
	: impl_(boost::make_shared<OmProjectCloseActionImpl>())
{
	SetUndoable(false);
}

void OmProjectCloseAction::Action()
{
	impl_->Execute();
}

void OmProjectCloseAction::UndoAction()
{
	impl_->Undo();
}

std::string OmProjectCloseAction::Description()
{
	return impl_->Description();
}

void OmProjectCloseAction::save(const std::string& comment)
{
	OmActionLogger::save(impl_, comment);
}


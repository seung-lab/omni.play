#include "datalayer/fs/omActionLoggerFS.h"
#include "omProjectSaveAction.h"
#include "actions/omProjectSaveActionImpl.hpp"

OmProjectSaveAction::OmProjectSaveAction()
	: impl_(boost::make_shared<OmProjectSaveActionImpl>())
{
	SetUndoable(false);
}

void OmProjectSaveAction::Action()
{
	impl_->Execute();
}

void OmProjectSaveAction::UndoAction()
{
	impl_->Undo();
}

std::string OmProjectSaveAction::Description()
{
	return impl_->Description();
}

void OmProjectSaveAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(impl_, comment);
}


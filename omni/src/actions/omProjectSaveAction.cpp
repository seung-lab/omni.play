#include "datalayer/fs/omActionLoggerFS.h"
#include "omProjectSaveAction.h"
#include "project/omProject.h"

/////////////////////////////////
///////
///////          OmProjectSaveAction
///////
OmProjectSaveAction::OmProjectSaveAction()
{
	SetUndoable(false);
}

/////////////////////////////////
///////          Action Methods
void OmProjectSaveAction::Action()
{
	OmProject::Save();
}

void OmProjectSaveAction::UndoAction()
{
}

std::string OmProjectSaveAction::Description()
{
	QString lineItem = QString("Saved");
	return lineItem.toStdString();
}

void OmProjectSaveAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(this, comment);
}


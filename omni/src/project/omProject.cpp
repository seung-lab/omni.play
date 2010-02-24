#include "omProject.h"
#include "system/omPreferenceDefinitions.h"

#include "volume/omVolume.h"
#include "segment/omSegmentEditor.h"
#include "system/omCacheManager.h"
#include "system/omEventManager.h"
#include "system/omGarbage.h"
#include "system/omKeyManager.h"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/omTagManager.h"
#include "system/omProjectData.h"

#include <QFile>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs = boost::filesystem;

#include "common/omDebug.h"
#include "common/omException.h"


#define DEBUG 0

static const string PROJECT_ARCHIVE_NAME = "project.dat";

//init instance pointer
OmProject *OmProject::mspInstance = 0;

#pragma mark -
#pragma mark OmProject
/////////////////////////////////
///////
///////          OmProject
///////

OmProject::OmProject()
{
}

OmProject::~OmProject()
{

}

OmProject *OmProject::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmProject;
	}
	return mspInstance;
}

void OmProject::Delete()
{
	if (mspInstance)
		delete mspInstance;
	mspInstance = NULL;
}




#pragma mark
#pragma mark Project IO
/////////////////////////////////
///////          Project IO

void OmProject::New( QString fileNameAndPath )
{
	OmProjectData::instantiateProjectData( fileNameAndPath );
	OmProjectData::Create();
	OmProjectData::Open();

	//load default project preferences
	omSetDefaultAllPreferences();
	OmKeyManager::SetDefaults();

	//save project
	//debug("genone","OmProject::New: save\n");
	Save();

	//debug("genone","OmProject::New: flushing\n");
	OmProjectData::Flush();
}

void OmProject::Save()
{

	if (!OmProjectData::IsOpen()) {
		return;
	}

	if (!OmProjectData::IsOpen()) {
		return;
	}

	//store archive
	OmProjectData::ArchiveWrite < OmProject > (PROJECT_ARCHIVE_NAME, Instance());
	OmProjectData::Flush();
}

void OmProject::Commit()
{
	Save();
	OmStateManager::ClearUndoStack();
}

void OmProject::Load( QString fileNameAndPath )
{
	QFile projectFile( fileNameAndPath );
	if( !projectFile.exists() ){
		QString err = "Project file not found at \"" + fileNameAndPath + "\"";
		throw OmIoException( qPrintable( err ));
	}
	
	OmProjectData::instantiateProjectData( fileNameAndPath );

	OmProjectData::Open();
	OmProjectData::ArchiveRead < OmProject > (PROJECT_ARCHIVE_NAME, Instance());
}

void OmProject::Close()
{
	//delete all singletons

	//volume depends on the following so it must be deleted first
	OmVolume::Delete();

	OmSegmentEditor::Delete();
	OmCacheManager::Delete();
	OmEventManager::Delete();
	OmGarbage::Delete();
	OmKeyManager::Delete();
	OmPreferences::Delete();
	OmStateManager::Delete();
	OmTagManager::Delete();

	//close project data
	OmProjectData::Close();
	OmProjectData::Delete();
}

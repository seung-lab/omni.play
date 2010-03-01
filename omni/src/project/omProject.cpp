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
#include <QFileInfo>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs = boost::filesystem;

#include "common/omDebug.h"
#include "common/omException.h"


#define DEBUG 0

//init instance pointer
OmProject *OmProject::mspInstance = 0;

/////////////////////////////////
///////          OmProject
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

/////////////////////////////////
///////          Project IO
void OmProject::New( QString fileNameAndPath )
{
	QFileInfo fileInfo( fileNameAndPath );
	Instance()->mFileName = fileInfo.fileName();
	Instance()->mDirectoryPath = fileInfo.filePath();

	OmProjectData::instantiateProjectData( fileNameAndPath );
	OmProjectData::Create();
	OmProjectData::Open();

	//load default project preferences
	omSetDefaultAllPreferences();
	OmKeyManager::SetDefaults();

	Save();

	OmProjectData::Flush();
}

void OmProject::Save()
{
	if (!OmProjectData::IsOpen()) {
		return;
	}

	OmProjectData::ArchiveWrite < OmProject > (OmHdf5Helpers::getProjectArchiveName(), Instance());
	OmProjectData::Flush();
}

void OmProject::Commit()
{
	Save();
	OmStateManager::ClearUndoStack();
}

void OmProject::Load( QString fileNameAndPath )
{
	QFileInfo fileInfo( fileNameAndPath );
	Instance()->mFileName = fileInfo.fileName();
	Instance()->mDirectoryPath = fileInfo.filePath();

	QFile projectFile( fileNameAndPath );
	if( !projectFile.exists() ){
		QString err = "Project file not found at \"" + fileNameAndPath + "\"";
		throw OmIoException( qPrintable( err ));
	}
	
	OmProjectData::instantiateProjectData( fileNameAndPath );

	OmProjectData::Open();
	OmProjectData::ArchiveRead < OmProject > (OmHdf5Helpers::getProjectArchiveName(), Instance());
	OmVolume::CheckDataResolution();
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

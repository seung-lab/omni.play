#include "omProject.h"
#include "system/omPreferenceDefinitions.h"

#include "volume/omVolume.h"
#include "segment/omSegmentEditor.h"
#include "utility/omDataArchiveQT.h"
#include "system/omCacheManager.h"
#include "system/omEventManager.h"
#include "system/omGarbage.h"
#include "system/omPreferences.h"
#include "system/omStateManager.h"
#include "system/omProjectData.h"

#include <QFile>
#include <QFileInfo>

#include "common/omDebug.h"
#include "common/omException.h"

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
QString OmProject::New( QString fileNameAndPath )
{
	if (!fileNameAndPath.endsWith(".omni")) {
		fileNameAndPath.append(".omni");
	}

	QFileInfo fileInfo( fileNameAndPath );
	Instance()->mFileName = fileInfo.fileName();
	Instance()->mDirectoryPath = fileInfo.filePath();

	OmProjectData::instantiateProjectData( fileNameAndPath, false );
	OmProjectData::Create();
	OmProjectData::Open();

	//load default project preferences
	omSetDefaultAllPreferences();
	
	Save();

	return fileInfo.absoluteFilePath();
}

void OmProject::Save()
{
	if (!OmProjectData::IsOpen()) {
		return;
	}

	//TODO: move this into omProjectData

	foreach( OmId segID, OmVolume::GetValidSegmentationIds() ){
		OmVolume::GetSegmentation( segID ).FlushDirtySegments();
	}

	OmDataArchiveQT::ArchiveWrite(OmHdf5Helpers::getProjectArchiveNameQT(), Instance());
	
}

void OmProject::Commit()
{
	Save();
	OmStateManager::ClearUndoStack();
}

void OmProject::Load( QString fileNameAndPath, const bool autoOpenAndClose )
{
	QFileInfo fileInfo( fileNameAndPath );
	Instance()->mFileName = fileInfo.fileName();
	Instance()->mDirectoryPath = fileInfo.filePath();

	QFile projectFile( fileNameAndPath );
	if( !projectFile.exists() ){
		QString err = "Project file not found at \"" + fileNameAndPath + "\"";
		throw OmIoException( qPrintable( err ));
	}
	
	OmProjectData::instantiateProjectData( fileNameAndPath, autoOpenAndClose );
	OmProjectData::Open();

	try {
		OmDataArchiveQT::ArchiveRead(OmHdf5Helpers::getProjectArchiveNameQT(), Instance());
	} catch( ... ) {
		OmProjectData::Close();
		throw OmIoException("error during load of project metadata");
	}

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
	OmPreferences::Delete();
	OmStateManager::Delete();

	//close project data
	OmProjectData::Close();
	OmProjectData::Delete();
}

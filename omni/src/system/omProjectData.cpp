#include "project/omProject.h"
#include "common/omDebug.h"
#include "common/omException.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omIDataWriter.h"
#include "segment/omSegment.h"
#include "tiles/cache/omTileCache.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omProjectData.h"
#include "utility/fileHelpers.h"

#include <QFile>
#include <QFileInfo>

//init instance pointer
OmProjectData *OmProjectData::mspInstance = NULL;

OmProjectData::OmProjectData()
	: fileVersion_(0)
	, mIsOpen(false)
	, mIsReadOnly(false)
	, tileCache_(new OmTileCache())
{
}

void OmProjectData::instantiateProjectData(const std::string& fileNameAndPath )
{
	if (NULL != mspInstance) {
		delete mspInstance;
		mspInstance = new OmProjectData();
	}

	Instance()->setupDataLayer( fileNameAndPath );
}

OmProjectData *OmProjectData::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmProjectData();
	}
	return mspInstance;
}

void OmProjectData::Delete()
{
	if (mspInstance){
		delete mspInstance;
	}
	mspInstance = NULL;
}

QString OmProjectData::getFileNameAndPath()
{
	return OmProject::GetFileNameAndPath();
}

QString OmProjectData::getAbsoluteFileNameAndPath()
{
	return OmProject::GetFileNameAndPath();
}

QString OmProjectData::getAbsolutePath()
{
	return OmProject::GetPath();
}

QDir OmProjectData::GetFilesFolderPath()
{
	if( "" == getAbsoluteFileNameAndPath()){
		throw OmIoException("project file name not set");
	}

	return QDir(getAbsoluteFileNameAndPath() + ".files");
}

/////////////////////////////////
///////          ProjectData Access

void OmProjectData::Create()
{
	QFile projectFile(getFileNameAndPath());
	if( projectFile.exists() ){
		projectFile.remove();
	}

	QDir filesDir = GetFilesFolderPath();
	const QString path = filesDir.absolutePath();
	if(filesDir.exists()){
		printf("removing folder %s...", qPrintable(path));
		fflush(stdout);
		if(FileHelpers::removeDir(path)){
			printf("done!\n");
		} else {
			throw OmIoException("could not remove folder", path);
		}
	}

	if(filesDir.mkpath(path)){
		printf("made folder \"%s\"\n", qPrintable(path));
	} else{
		throw OmIoException("could not make folder", path);
	}

	Instance()->dataWriter->create();
}

void OmProjectData::Open()
{
	Instance()->dataReader->open();
	Instance()->mIsOpen = true;
}

void OmProjectData::Close()
{
	if(!IsOpen()){
		return;
	}
	Instance()->dataReader->close();
	Instance()->mIsOpen = false;
}

/**
 *	Deletes all data on disk associated with path if it exists.
 */
void OmProjectData::DeleteInternalData(const OmDataPath & path)
{
	//TODO: mutex lock this?
	if (OmProjectData::GetProjectIDataReader()->group_exists(path)) {
		OmProjectData::GetIDataWriter()->group_delete(path);
	}
}

OmIDataReader* OmProjectData::GetProjectIDataReader()
{
	return Instance()->dataReader;
}

OmIDataWriter* OmProjectData::GetIDataWriter()
{
	return Instance()->dataWriter;
}

void OmProjectData::setupDataLayer(const std::string& fileNameAndPath)
{
	mIsReadOnly = FileHelpers::isFileReadOnly(fileNameAndPath);
	dataReader = OmDataLayer::getReader(fileNameAndPath, mIsReadOnly);
	dataWriter = OmDataLayer::getWriter(fileNameAndPath, mIsReadOnly);
}

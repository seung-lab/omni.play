#include "common/omDebug.h"
#include "common/omException.h"
#include "datalayer/hdf5/omHdf5Manager.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "omPreferenceDefinitions.h"
#include "omProjectData.h"
#include "segment/omSegment.h"
#include "utility/fileHelpers.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"

#include <QFile>
#include <QFileInfo>

//init instance pointer
OmProjectData *OmProjectData::mspInstance = NULL;

OmProjectData::OmProjectData()
	: mIsReadOnly(false)
{
}

void OmProjectData::instantiateProjectData( QString fileNameAndPath )
{
	if (NULL != mspInstance) {
		delete mspInstance;
		mspInstance = new OmProjectData();
	}

	Instance()->setupDataLayer( fileNameAndPath );
}

OmProjectData::~OmProjectData()
{
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
	if (mspInstance)
		delete mspInstance;
	mspInstance = NULL;
}

QString OmProjectData::getFileNameAndPath()
{
	return Instance()->dataReader->getFileNameAndPath();
}

QString OmProjectData::getAbsoluteFileNameAndPath()
{
	QString rel_fnpn = Instance()->getFileNameAndPath();
	QFileInfo fInfo(rel_fnpn);
	return fInfo.absoluteFilePath();
}

QString OmProjectData::getAbsolutePath()
{
	QString rel_fnpn = Instance()->getFileNameAndPath();
	QFileInfo fInfo(rel_fnpn);
	return fInfo.absolutePath();
}

/////////////////////////////////
///////          ProjectData Access

void OmProjectData::Create()
{
	QFile projectFile( getFileNameAndPath() );
	if( projectFile.exists() ){
		projectFile.remove();
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
	//TODO: mutex lock this!!!!
	if (OmProjectData::GetProjectDataReader()->group_exists(path)) {
		OmProjectData::GetDataWriter()->group_delete(path);
	}
}

OmIDataReader* OmProjectData::GetProjectDataReader()
{
	return Instance()->dataReader;
}

OmIDataWriter* OmProjectData::GetDataWriter()
{
	return Instance()->dataWriter;
}

void OmProjectData::setupDataLayer( QString fileNameAndPath )
{
	mIsReadOnly = FileHelpers::isFileReadOnly(fileNameAndPath);
	dataReader = OmDataLayer::getReader(fileNameAndPath, mIsReadOnly);
	dataWriter = OmDataLayer::getWriter(fileNameAndPath, mIsReadOnly);
}

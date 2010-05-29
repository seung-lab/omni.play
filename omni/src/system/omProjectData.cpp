#include "common/omDebug.h"
#include "common/omException.h"
#include "omPreferenceDefinitions.h"
#include "omProjectData.h"
#include "segment/omSegment.h"
#include "utility/fileHelpers.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omHdf5Manager.h"

#include <QFile>
#include <QFileInfo>

//init instance pointer
OmProjectData *OmProjectData::mspInstance = 0;


/////////////////////////////////
///////
///////          OmProjectData
///////

OmProjectData::OmProjectData()
{
	mIsReadOnly = false;
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
	Instance()->dataReader->close();
	Instance()->mIsOpen = false;
}

OmDataLayer * OmProjectData::GetDataLayer()
{
	return Instance()->dataLayer;
}

OmDataReader * OmProjectData::GetProjectDataReader()
{
	return Instance()->dataReader;
}

OmDataWriter * OmProjectData::GetDataWriter()
{
	return Instance()->dataWriter;
}

void OmProjectData::setupDataLayer( QString fileNameAndPath )
{
	dataLayer = new OmDataLayer();
	mIsReadOnly = FileHelpers::isFileReadOnly( fileNameAndPath);
	dataReader = dataLayer->getReader( fileNameAndPath, mIsReadOnly );
	dataWriter = dataLayer->getWriter( fileNameAndPath, mIsReadOnly );
}

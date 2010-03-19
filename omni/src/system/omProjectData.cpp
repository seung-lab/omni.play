#include "omProjectData.h"
#include "omPreferenceDefinitions.h"
#include "common/omDebug.h"
#include "common/omException.h"
#include "utility/omHdf5Manager.h"
#include <QFileInfo>
#include <QFile>

//init instance pointer
OmProjectData *OmProjectData::mspInstance = 0;

/////////////////////////////////
///////
///////          OmProjectData
///////

OmProjectData::OmProjectData()
{
}

void OmProjectData::instantiateProjectData( QString fileNameAndPath, const bool autoOpenAndClose )
{
	if (NULL != mspInstance) {
		delete mspInstance;
		mspInstance = new OmProjectData;
	}

	Instance()->dataLayer = new OmDataLayer();

	bool isReadOnly = true;
	QFileInfo file(fileNameAndPath);
	if (file.permission(QFile::WriteUser)){
		isReadOnly = false; 
	}

	Instance()->dataReader = Instance()->dataLayer->getReader( fileNameAndPath, autoOpenAndClose, isReadOnly );
	Instance()->dataWriter = Instance()->dataLayer->getWriter( fileNameAndPath, autoOpenAndClose );
}

OmProjectData::~OmProjectData()
{
}

OmProjectData *OmProjectData::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmProjectData;
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

void OmProjectData::Flush()
{
	Close();
	Open();
}

/////////////////////////////////
///////          ProjectData IO

//groups

bool OmProjectData::GroupExists(OmHdf5Path path)
{
	return Instance()->dataReader->group_exists( path );
}

void OmProjectData::GroupDelete(OmHdf5Path path)
{
	Instance()->dataWriter->group_delete( path );
}

bool OmProjectData::DataExists(OmHdf5Path path)
{
	return Instance()->dataReader->dataset_exists( path );
}

//image data io

void OmProjectData::CreateImageData(OmHdf5Path path, Vector3<int>* dataDims, Vector3<int>* chunkDims,
				    int bytesPerSample)
{
	Instance()->dataWriter->dataset_image_create_tree_overwrite( path, dataDims, chunkDims, bytesPerSample);
}

vtkImageData *OmProjectData::ReadImageData(OmHdf5Path path, const DataBbox & extent, int bytesPerSample)
{
	return Instance()->dataReader->dataset_image_read_trim( path, extent, bytesPerSample);
}

void OmProjectData::WriteImageData(OmHdf5Path path, DataBbox * extent, int bytesPerSample, vtkImageData * data)
{
	Instance()->dataWriter->dataset_image_write_trim( path, extent, bytesPerSample, data);
}

//raw data io
void *OmProjectData::ReadRawData(OmHdf5Path path, int *size)
{
	return Instance()->dataReader->dataset_raw_read( path, size);
}

void OmProjectData::WriteRawData(OmHdf5Path path, int size, const void *data)
{
	Instance()->dataWriter->dataset_raw_create_tree_overwrite( path, size, data);
}

OmDataLayer * OmProjectData::GetDataLayer()
{
	return Instance()->dataLayer;
}

OmDataReader * OmProjectData::GetDataReader()
{
	return Instance()->dataReader;
}

OmDataWriter * OmProjectData::GetDataWriter()
{
	return Instance()->dataWriter;
}

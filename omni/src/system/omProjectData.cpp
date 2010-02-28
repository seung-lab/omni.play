#include "omProjectData.h"
#include "omPreferenceDefinitions.h"
#include "common/omDebug.h"
#include "common/omException.h"
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

void OmProjectData::instantiateProjectData( QString fileNameAndPath )
{
	if (NULL != mspInstance) {
		delete mspInstance;
		mspInstance = new OmProjectData;
	}

	Instance()->hdfFile = new OmHdf5( fileNameAndPath, false );
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
	return Instance()->hdfFile->getFileNameAndPath();
}

/////////////////////////////////
///////          ProjectData Access

void OmProjectData::ResetHDF5fileAsAutoOpenAndClose( const bool autoOpenAndClose )
{
	Instance()->hdfFile->resetHDF5fileAsAutoOpenAndClose( autoOpenAndClose );
}

void OmProjectData::Create()
{
	QFile projectFile( getFileNameAndPath() );
	if( projectFile.exists() ){
		projectFile.remove();
	}

	Instance()->hdfFile->create();
}

void OmProjectData::Open()
{
	Instance()->hdfFile->open();
	Instance()->mIsOpen = true;
}

void OmProjectData::Close()
{
	Instance()->hdfFile->close();
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

bool OmProjectData::GroupExists(string & path)
{
	return Instance()->hdfFile->group_exists( path );
}

void OmProjectData::GroupDelete(string & path)
{
	Instance()->hdfFile->group_delete( path );
}

bool OmProjectData::DataExists(string & path)
{
	return Instance()->hdfFile->dataset_exists( path );
}

//image data io

void OmProjectData::CreateImageData(string & path, Vector3 < int >dataDims, Vector3 < int >chunkDims,
				    int bytesPerSample)
{
	Instance()->hdfFile->dataset_image_create_tree_overwrite( path, dataDims, chunkDims, bytesPerSample);
}

vtkImageData *OmProjectData::ReadImageData(string & path, const DataBbox & extent, int bytesPerSample)
{
	return Instance()->hdfFile->dataset_image_read_trim( path, extent, bytesPerSample);
}

void OmProjectData::WriteImageData(string & path, const DataBbox & extent, int bytesPerSample, vtkImageData * data)
{
	Instance()->hdfFile->dataset_image_write_trim( path, extent, bytesPerSample, data);
}

//raw data io
void *OmProjectData::ReadRawData(string & path, int *size)
{
	return Instance()->hdfFile->dataset_raw_read( path, size);
}

void OmProjectData::WriteRawData(string & path, int size, const void *data)
{
	Instance()->hdfFile->dataset_raw_create_tree_overwrite( path, size, data);
}



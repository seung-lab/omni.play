
#include "omProjectData.h"

#include "omPreferenceDefinitions.h"

#include "common/omDebug.h"
#include "common/omException.h"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
namespace bfs = boost::filesystem;


#define DEBUG 0

//init instance pointer
OmProjectData *OmProjectData::mspInstance = 0;

#pragma mark -
#pragma mark OmProjectData
/////////////////////////////////
///////
///////          OmProjectData
///////

OmProjectData::OmProjectData()
{
	mFileId = -1;
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

#pragma mark
#pragma mark ProjectData Access
/////////////////////////////////
///////          ProjectData Access

string OmProjectData::GetFilePath()
{
	return GetDirectoryPath() + GetFileName();
}

void OmProjectData::Create()
{
	//debug("genone","OmProjectData::Create()");

	string fpath = GetFilePath();

	//delete file if already exists
	if (bfs::exists(bfs::path(fpath))) {
		bfs::remove(bfs::path(fpath));
	}

	om_hdf5_file_create(fpath.c_str());
}

void OmProjectData::Open()
{
	//debug("genone","OmProjectData::Open()");
	assert(!IsOpen());
	Instance()->mFileId = om_hdf5_file_open(GetFilePath().c_str());
}

void OmProjectData::Close()
{
	//debug("genone","OmProjectData::Close()");
	assert(IsOpen());

	om_hdf5_file_close(Instance()->mFileId);
	Instance()->mFileId = -1;
}

void OmProjectData::Flush()
{
	//debug("genone","OmProjectData::Flush()");
	assert(IsOpen());
	Close();
	Open();
}

bool OmProjectData::IsOpen()
{
	return (Instance()->mFileId >= 0);
}





#pragma mark
#pragma mark Project
/////////////////////////////////
///////          Project

//project
const string & OmProjectData::GetFileName()
{
	return Instance()->mFileName;
}

void OmProjectData::SetFileName(const string & fname)
{
	Instance()->mFileName = fname;
}

const string & OmProjectData::GetDirectoryPath()
{
	return Instance()->mDirectoryPath;
}

void OmProjectData::SetDirectoryPath(const string & dpath)
{
	Instance()->mDirectoryPath = dpath;
}

string OmProjectData::GetTempDirectoryPath()
{
	string dpath = GetDirectoryPath();
	return dpath + "temp/";
}







#pragma mark
#pragma mark ProjectData IO
/////////////////////////////////
///////          ProjectData IO

//groups

bool OmProjectData::GroupExists(string & path)
{
	assert(IsOpen());
	return om_hdf5_group_exists(Instance()->mFileId, path.c_str());
}

void OmProjectData::GroupDelete(string & path)
{
	assert(IsOpen());
	om_hdf5_group_delete(Instance()->mFileId, path.c_str());
}

bool OmProjectData::DataExists(string & path)
{
	assert(IsOpen());
	om_hdf5_dataset_exists(Instance()->mFileId, path.c_str());
}

//image data io

void OmProjectData::CreateImageData(string & path, Vector3 < int >dataDims, Vector3 < int >chunkDims,
				    int bytesPerSample)
{
	assert(IsOpen());

	om_hdf5_dataset_image_create_tree_overwrite(Instance()->mFileId, path.c_str(),
						    dataDims, chunkDims, bytesPerSample);

	//debug("FIXME", << "OmProjectData::CreateImageData: " << om_hdf5_dataset_image_get_dims(Instance()->mFileId, path.c_str()) << endl;
}

vtkImageData *OmProjectData::ReadImageData(string & path, const DataBbox & extent, int bytesPerSample)
{
	assert(IsOpen());
	return om_hdf5_dataset_image_read_trim(Instance()->mFileId, path.c_str(), extent, bytesPerSample);
}

void OmProjectData::WriteImageData(string & path, const DataBbox & extent, int bytesPerSample, vtkImageData * data)
{
	assert(IsOpen());
	om_hdf5_dataset_image_write_trim(Instance()->mFileId, path.c_str(), extent, bytesPerSample, data);
}

//raw data io
void *OmProjectData::ReadRawData(string & path, int *size)
{
	assert(IsOpen());
	return om_hdf5_dataset_raw_read(Instance()->mFileId, path.c_str(), size);
}

void OmProjectData::WriteRawData(string & path, int size, const void *data)
{
	assert(IsOpen());
	om_hdf5_dataset_raw_create_tree_overwrite(Instance()->mFileId, path.c_str(), size, data);
}

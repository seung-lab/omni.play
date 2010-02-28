#include "omHdf5.h"
#include <stdlib.h>

OmHdf5::OmHdf5( QString fileNameAndPath, const bool keepOpen )
{
	m_fileNameAndPath = fileNameAndPath;
	fileLock = new QMutex();

	if( keepOpen ){
		hdfLowLevelWrap = new OmHdf5LowLevelWrappersAlwaysClose();
	} else {
		hdfLowLevelWrap = new OmHdf5LowLevelWrappersAlwaysClose();
	}
}

OmHdf5::~OmHdf5()
{
	delete hdfLowLevelWrap;
}

// filename and path accessors
QString OmHdf5::getFileNameAndPath()
{
	return m_fileNameAndPath;
}

string OmHdf5::getFileNameAndPathString()
{
	return m_fileNameAndPath.toStdString();
}

// hdf5 wrappers -- no locking
void OmHdf5::create()
{
	hdfLowLevelWrap->file_create( getFileNameAndPathString() );
}

// hdf5 wrappers -- with locking
void OmHdf5::open()
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->open(getFileNameAndPathString());
}

void OmHdf5::close()
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->close(getFileNameAndPathString());
}

bool OmHdf5::group_exists( string name )
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->group_exists_with_lock(getFileNameAndPathString(), name.c_str());
}

void OmHdf5::group_delete( string name )
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->group_delete_with_lock(getFileNameAndPathString(), name.c_str());
}

bool OmHdf5::dataset_exists( string name )
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_exists_with_lock(getFileNameAndPathString(), name.c_str());
}

void OmHdf5::dataset_image_create_tree_overwrite( string & name, Vector3 < int >dataDims, 
						  Vector3 < int >chunkDims, int bytesPerSample,
						  bool unlimited ) 
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->dataset_image_create_tree_overwrite_with_lock(getFileNameAndPathString(), name.c_str(), dataDims, chunkDims, bytesPerSample, unlimited );
}

vtkImageData* OmHdf5::dataset_image_read_trim( string name, DataBbox dataExtent, int bytesPerSample)
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_image_read_trim_with_lock(getFileNameAndPathString(), name.c_str(), dataExtent, bytesPerSample);
}

void OmHdf5::dataset_image_write_trim( string name, DataBbox dataExtent, 
				       int bytesPerSample, vtkImageData *pImageData)
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->dataset_image_write_trim_with_lock(getFileNameAndPathString(), name.c_str(), 
					 dataExtent, bytesPerSample, pImageData);
}

void* OmHdf5::dataset_raw_read( string name, int* size)
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_raw_read_with_lock(getFileNameAndPathString(), name.c_str(), size);
}

void OmHdf5::dataset_raw_create_tree_overwrite( string name, int size, const void* data)
{
	if (!size){
		return;
	}
	
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->dataset_raw_create_tree_overwrite_with_lock(getFileNameAndPathString(), name.c_str(), size, data);
}

Vector3 < int > OmHdf5::dataset_image_get_dims( string name )
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->dataset_image_get_dims_with_lock(getFileNameAndPathString(), name.c_str());
}

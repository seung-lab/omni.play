#include "omHdf5.h"
#include "common/omDebug.h"
#include <stdlib.h>

OmHdf5::OmHdf5( QString fileNameAndPath, const bool autoOpenAndClose )
{
	m_fileNameAndPath = fileNameAndPath;
	fileLock = new QMutex();
	setHDF5fileAsAutoOpenAndClose( autoOpenAndClose );
}

OmHdf5::~OmHdf5()
{
	debug ("meshercrash", "deleting %p %s\n", this, qPrintable(m_fileNameAndPath));
	delete hdfLowLevelWrap;
}

// filename and path accessors
QString OmHdf5::getFileNameAndPath()
{
	return m_fileNameAndPath;
}

string OmHdf5::getFileNameAndPathString()
{
	debug ("meshercrash", "%s\n", m_fileNameAndPath.toStdString().c_str());
	return m_fileNameAndPath.toStdString();
}

void OmHdf5::setHDF5fileAsAutoOpenAndClose( const bool autoOpenAndClose )
{
	if( autoOpenAndClose ){
		hdfLowLevelWrap = new OmHdf5LowLevelWrappersAutoOpenClose(getFileNameAndPathString());
	} else {
		hdfLowLevelWrap = new OmHdf5LowLevelWrappersManualOpenClose(getFileNameAndPathString());
	}
}

void OmHdf5::resetHDF5fileAsAutoOpenAndClose( const bool autoOpenAndClose )
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->close();
	delete(hdfLowLevelWrap);
	setHDF5fileAsAutoOpenAndClose( autoOpenAndClose );
}

// hdf5 wrappers -- no locking
void OmHdf5::create()
{
	hdfLowLevelWrap->file_create();
}

// hdf5 wrappers -- with locking
void OmHdf5::open()
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->open();
}

void OmHdf5::close()
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->close();
}

bool OmHdf5::group_exists( string name )
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->group_exists_with_lock(name.c_str());
}

void OmHdf5::group_delete( string name )
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->group_delete_with_lock(name.c_str());
}

bool OmHdf5::dataset_exists( string name )
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_exists_with_lock(name.c_str());
}

void OmHdf5::dataset_image_create_tree_overwrite( string & name, Vector3 < int >dataDims, 
						  Vector3 < int >chunkDims, int bytesPerSample,
						  bool unlimited ) 
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->dataset_image_create_tree_overwrite_with_lock(name.c_str(), dataDims, chunkDims, bytesPerSample, unlimited );
}

vtkImageData* OmHdf5::dataset_image_read_trim( string name, DataBbox dataExtent, int bytesPerSample)
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_image_read_trim_with_lock(name.c_str(), dataExtent, bytesPerSample);
}

void OmHdf5::dataset_image_write_trim( string name, DataBbox dataExtent, 
				       int bytesPerSample, vtkImageData *pImageData)
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->dataset_image_write_trim_with_lock(name.c_str(), dataExtent, bytesPerSample, pImageData);
}

void* OmHdf5::dataset_raw_read( string name, int* size)
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_raw_read_with_lock(name.c_str(), size);
}

void OmHdf5::dataset_raw_create_tree_overwrite( string name, int size, const void* data)
{
	if (!size){
		return;
	}
	
	debug ("meshercrash", "%p, %s, %i, %p\n", this, name.c_str(), size, data);
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->dataset_raw_create_tree_overwrite_with_lock(name.c_str(), size, data);
}

Vector3 < int > OmHdf5::dataset_image_get_dims( string name )
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->dataset_image_get_dims_with_lock(name.c_str());
}

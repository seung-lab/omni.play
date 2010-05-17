#include "utility/omHdf5.h"
#include "common/omDebug.h"
#include <stdlib.h>

OmHdf5::OmHdf5( QString fileNameAndPath, const bool autoOpenAndClose, const bool readOnly )
{
	m_fileNameAndPath = fileNameAndPath;
	fileLock = new QMutex();
	setHDF5fileAsAutoOpenAndClose( autoOpenAndClose, readOnly );
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

// set whether HDF file should be kept opened, 
//  or automatically closed, between calls into the file
void OmHdf5::setHDF5fileAsAutoOpenAndClose( const bool autoOpenAndClose, const bool readOnly )
{
	if( autoOpenAndClose ){
		hdfLowLevelWrap = new OmHdf5LowLevelWrappersAutoOpenClose(getFileNameAndPathString(), readOnly);
	} else {
		hdfLowLevelWrap = new OmHdf5LowLevelWrappersManualOpenClose(getFileNameAndPathString(), readOnly);
	}
}

// hdf5 wrappers -- no locking
void OmHdf5::create()
{
	QMutexLocker locker(fileLock);
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

bool OmHdf5::group_exists( OmHdf5Path path )
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->group_exists_with_lock( path );
}

void OmHdf5::group_delete( OmHdf5Path path )
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->group_delete_with_lock( path );
}

bool OmHdf5::dataset_exists( OmHdf5Path path )
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_exists_with_lock( path );
}

void OmHdf5::dataset_image_create_tree_overwrite( OmHdf5Path path, Vector3<int>* dataDims, 
						  Vector3<int>* chunkDims, int bytesPerSample ) 
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->dataset_image_create_tree_overwrite_with_lock( path, dataDims, chunkDims, bytesPerSample);
}

vtkImageData* OmHdf5::dataset_image_read_trim( OmHdf5Path path, DataBbox dataExtent, int bytesPerSample)
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_image_read_trim_with_lock( path, dataExtent, bytesPerSample);
}

void OmHdf5::dataset_image_write_trim( OmHdf5Path path, DataBbox *dataExtent, 
				       int bytesPerSample, vtkImageData *pImageData)
{
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->dataset_image_write_trim_with_lock( path, dataExtent, bytesPerSample, pImageData);
}

void* OmHdf5::dataset_raw_read( OmHdf5Path path, int* size)
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_raw_read_with_lock( path, size);
}

void* OmHdf5::dataset_read_raw_chunk_data( OmHdf5Path path, DataBbox dataExtent, int bytesPerSample)
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_read_raw_chunk_data( path, dataExtent, bytesPerSample );

}

void OmHdf5::dataset_write_raw_chunk_data(OmHdf5Path path, DataBbox dataExtent, int bytesPerSample, void * imageData)
{

	hdfLowLevelWrap->dataset_write_raw_chunk_data(path, dataExtent, bytesPerSample, imageData);

}

void OmHdf5::dataset_raw_create_tree_overwrite( OmHdf5Path path, int size, const void* data)
{
	if (!size){
		return;
	}
	
	debug ("meshercrash", "%p, %s, %i, %p\n", this, path.getString().c_str(), size, data);
	QMutexLocker locker(fileLock);
	hdfLowLevelWrap->dataset_raw_create_tree_overwrite_with_lock( path, size, data);
}

Vector3 < int > OmHdf5::dataset_image_get_dims( OmHdf5Path path )
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_image_get_dims_with_lock( path );
}

Vector3< int > OmHdf5::dataset_get_dims( OmHdf5Path path )
{
	QMutexLocker locker(fileLock);
	return hdfLowLevelWrap->dataset_get_dims_with_lock( path );
}

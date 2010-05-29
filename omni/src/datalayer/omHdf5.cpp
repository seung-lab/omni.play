#include "datalayer/omHdf5.h"
#include "common/omDebug.h"
#include <stdlib.h>
#include "datalayer/omHdf5LowLevelWrappersManualOpenClose.h"

OmHdf5::OmHdf5( QString fileNameAndPath, const bool readOnly )
	: m_fileNameAndPath(fileNameAndPath)
	, hdfLowLevelWrap( new OmHdf5LowLevelWrappersManualOpenClose(getFileNameAndPathString(), readOnly) )
{
}

OmHdf5::~OmHdf5()
{
	delete hdfLowLevelWrap;
}

QString OmHdf5::getFileNameAndPath()
{
	return m_fileNameAndPath;
}

string OmHdf5::getFileNameAndPathString()
{
	return m_fileNameAndPath.toStdString();
}

void OmHdf5::create()
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->file_create();
}

void OmHdf5::open()
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->open();
}

void OmHdf5::close()
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->close();
}

void OmHdf5::flush()
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->flush();
}

bool OmHdf5::group_exists( const OmHdf5Path & path )
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->group_exists_with_lock( path );
}

void OmHdf5::group_delete( const OmHdf5Path & path )
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->group_delete_with_lock( path );
}

bool OmHdf5::dataset_exists( const OmHdf5Path & path )
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_exists_with_lock( path );
}

void OmHdf5::dataset_image_create_tree_overwrite( const OmHdf5Path & path, Vector3<int>* dataDims, 
						  Vector3<int>* chunkDims, int bytesPerSample ) 
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->dataset_image_create_tree_overwrite_with_lock( path, dataDims, chunkDims, bytesPerSample);
}

vtkImageData* OmHdf5::dataset_image_read_trim( const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample)
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_image_read_trim_with_lock( path, dataExtent, bytesPerSample);
}

void OmHdf5::dataset_image_write_trim( const OmHdf5Path & path, DataBbox *dataExtent, 
				       int bytesPerSample, vtkImageData *pImageData)
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->dataset_image_write_trim_with_lock( path, dataExtent, bytesPerSample, pImageData);
}

void* OmHdf5::dataset_raw_read( const OmHdf5Path & path, int* size)
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_raw_read_with_lock( path, size);
}

void* OmHdf5::dataset_read_raw_chunk_data( const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample)
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_read_raw_chunk_data( path, dataExtent, bytesPerSample );
}

void OmHdf5::dataset_write_raw_chunk_data(const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample, void * imageData)
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->dataset_write_raw_chunk_data(path, dataExtent, bytesPerSample, imageData);
}

void OmHdf5::dataset_raw_create_tree_overwrite( const OmHdf5Path & path, int size, const void* data)
{
	if (!size){
		return;
	}
	
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->dataset_raw_create_tree_overwrite_with_lock( path, size, data);
}

Vector3 < int > OmHdf5::dataset_image_get_dims( const OmHdf5Path & path )
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_image_get_dims_with_lock( path );
}

Vector3< int > OmHdf5::dataset_get_dims( const OmHdf5Path & path )
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_get_dims_with_lock( path );
}

#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5LowLevelWrappersManualOpenClose.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"

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

bool OmHdf5::group_exists( const OmDataPath & path )
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->group_exists_with_lock( path );
}

void OmHdf5::group_delete( const OmDataPath & path )
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->group_delete_with_lock( path );
}

bool OmHdf5::dataset_exists( const OmDataPath & path )
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_exists_with_lock( path );
}

void OmHdf5::dataset_image_create_tree_overwrite( const OmDataPath & path,
						  const Vector3i& dataDims,
						  const Vector3i& chunkDims,
						  const OmAllowedVolumeDataTypes type)
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->dataset_image_create_tree_overwrite_with_lock( path, dataDims, chunkDims, type);
}

OmDataWrapperPtr OmHdf5::dataset_image_read_trim( const OmDataPath & path, DataBbox dataExtent)
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_image_read_trim_with_lock( path, dataExtent);
}

void OmHdf5::dataset_image_write_trim( const OmDataPath & path, DataBbox *dataExtent, OmDataWrapperPtr data)
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->dataset_image_write_trim_with_lock( path, dataExtent, data);
}

OmDataWrapperPtr OmHdf5::dataset_raw_read( const OmDataPath & path, int* size)
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_raw_read_with_lock( path, size);
}

OmDataWrapperPtr OmHdf5::dataset_read_raw_chunk_data( const OmDataPath & path,
						      DataBbox dataExtent)
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_read_raw_chunk_data( path, dataExtent);
}

void OmHdf5::dataset_write_raw_chunk_data(const OmDataPath & path,
					  DataBbox dataExtent,
					  OmDataWrapperPtr data)
{
	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->dataset_write_raw_chunk_data(path, dataExtent, data);
}

void OmHdf5::dataset_raw_create_tree_overwrite( const OmDataPath & path,
						int size,
						const OmDataWrapperPtr data)
{
	if (!size){
		return;
	}

	QMutexLocker locker(&fileLock);
	hdfLowLevelWrap->dataset_raw_create_tree_overwrite_with_lock( path, size, data);
}

Vector3 < int > OmHdf5::dataset_image_get_dims( const OmDataPath & path )
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_image_get_dims_with_lock( path );
}

Vector3< int > OmHdf5::dataset_get_dims( const OmDataPath & path )
{
	QMutexLocker locker(&fileLock);
	return hdfLowLevelWrap->dataset_get_dims_with_lock( path );
}

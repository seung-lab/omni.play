#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5LowLevelWrappersManualOpenClose.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"

OmHdf5::OmHdf5( QString fileNameAndPath, const bool readOnly )
	: m_fileNameAndPath(fileNameAndPath)
	, hdfLowLevelWrap(new OmHdf5LowLevelWrappersManualOpenClose(getFileNameAndPathString(), readOnly) )
{
}

OmHdf5::~OmHdf5()
{
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
	zi::Guard g(fileLock);
	hdfLowLevelWrap->file_create();
}

void OmHdf5::open()
{
	zi::Guard g(fileLock);
	hdfLowLevelWrap->open();
}

void OmHdf5::close()
{
	zi::Guard g(fileLock);
	hdfLowLevelWrap->close();
}

void OmHdf5::flush()
{
	zi::Guard g(fileLock);
	hdfLowLevelWrap->flush();
}

bool OmHdf5::group_exists( const OmDataPath & path )
{
	zi::Guard g(fileLock);
	return hdfLowLevelWrap->group_exists( path );
}

void OmHdf5::group_delete( const OmDataPath & path )
{
	zi::Guard g(fileLock);
	hdfLowLevelWrap->group_delete( path );
}

bool OmHdf5::dataset_exists( const OmDataPath & path )
{
	zi::Guard g(fileLock);
	return hdfLowLevelWrap->dataset_exists( path );
}

void OmHdf5::dataset_image_create_tree_overwrite( const OmDataPath & path,
						  const Vector3i& dataDims,
						  const Vector3i& chunkDims,
						  const OmVolDataType type)
{
	zi::Guard g(fileLock);
	hdfLowLevelWrap->dataset_image_create_tree_overwrite( path, dataDims, chunkDims, type);
}

OmDataWrapperPtr OmHdf5::dataset_image_read_trim( const OmDataPath & path, DataBbox dataExtent)
{
	zi::Guard g(fileLock);
	return hdfLowLevelWrap->dataset_image_read_trim( path, dataExtent);
}

OmDataWrapperPtr OmHdf5::dataset_raw_read( const OmDataPath & path, int* size)
{
	zi::Guard g(fileLock);
	return hdfLowLevelWrap->dataset_raw_read( path, size);
}

OmDataWrapperPtr OmHdf5::dataset_read_raw_chunk_data( const OmDataPath & path,
						      DataBbox dataExtent)
{
	zi::Guard g(fileLock);
	return hdfLowLevelWrap->dataset_read_raw_chunk_data( path, dataExtent);
}

void OmHdf5::dataset_write_raw_chunk_data(const OmDataPath & path,
					  DataBbox dataExtent,
					  OmDataWrapperPtr data)
{
	zi::Guard g(fileLock);
	hdfLowLevelWrap->dataset_write_raw_chunk_data(path, dataExtent, data);
}

void OmHdf5::dataset_raw_create_tree_overwrite( const OmDataPath & path,
						int size,
						const OmDataWrapperPtr data)
{
	if (!size){
		return;
	}

	zi::Guard g(fileLock);
	hdfLowLevelWrap->dataset_raw_create_tree_overwrite( path, size, data);
}

Vector3i OmHdf5::dataset_image_get_dims( const OmDataPath & path )
{
	zi::Guard g(fileLock);
	return hdfLowLevelWrap->dataset_image_get_dims( path );
}

Vector3i OmHdf5::dataset_get_dims( const OmDataPath & path )
{
	zi::Guard g(fileLock);
	return hdfLowLevelWrap->dataset_get_dims( path );
}

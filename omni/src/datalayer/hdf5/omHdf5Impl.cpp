#include "common/omDebug.h"
#include "common/omException.h"
#include "datalayer/hdf5/omHdf5Impl.h"
#include "datalayer/omDataPath.h"
#include "datalayer/hdf5/omHdf5Utils.hpp"

#define HDF5_CHECK() 						\
	string pathStr = path.getString();                      \
	const char * name = pathStr.c_str();                    \
	if(!opened) {                                           \
                throw OmIoException("HDF5 file was not open");  \
	}

OmHdf5Impl::OmHdf5Impl(string fileName, const bool readOnly)
	: mFileName(fileName)
	, mReadOnly(readOnly)
	, fileId(-1)
	, opened(false)
{
}

OmHdf5Impl::~OmHdf5Impl()
{
	close();
}

void OmHdf5Impl::open()
{
	if(opened) {
		assert(0);
                throw OmIoException("HDF5 file was already open");
	}

	fileId = OmHdf5FileUtils::file_open(mFileName, mReadOnly);
	opened = true;
	debug("hdf5verbose","Real HDF5 id is %d\n", fileId);
}

void OmHdf5Impl::close()
{
	if(!opened) {
		return;
	}

	OmHdf5FileUtils::file_close(fileId);
	opened = false;
}

/////////////////////////////////
///////          File
void OmHdf5Impl::file_create()
{
	OmHdf5FileUtils::file_create( mFileName );
}

void OmHdf5Impl::flush()
{
	OmHdf5FileUtils::flush(fileId);
}

/////////////////////////////////
///////          Group
bool OmHdf5Impl::group_exists(const OmDataPath & path)
{
	HDF5_CHECK();
	return hdfLowLevel.group_exists(fileId, name);
}

void OmHdf5Impl::group_delete(const OmDataPath & path)
{
	HDF5_CHECK();
	hdfLowLevel.group_delete(fileId, name);
}

/////////////////////////////////
///////          Dataset

bool OmHdf5Impl::dataset_exists(const OmDataPath & path)
{
	HDF5_CHECK();
	return hdfLowLevel.dataset_exists(fileId, name);
}

void OmHdf5Impl::
dataset_image_create_tree_overwrite(const OmDataPath & path,
					      const Vector3i& dataDims,
					      const Vector3i& chunkDims,
					      const OmVolDataType type)
{
	HDF5_CHECK();
	hdfLowLevel.dataset_delete_create_tree(fileId, name);
	hdfLowLevel.dataset_image_create(fileId, name, dataDims, chunkDims, type);
}

OmDataWrapperPtr OmHdf5Impl::dataset_image_read_trim(const OmDataPath & path, DataBbox dataExtent)
{
	HDF5_CHECK();
	return hdfLowLevel.dataset_image_read_trim(fileId, name, dataExtent);
}

OmDataWrapperPtr OmHdf5Impl::dataset_raw_read(const OmDataPath & path, int *size)
{
	HDF5_CHECK();
	return hdfLowLevel.dataset_raw_read(fileId, name, size);
}

void OmHdf5Impl::dataset_raw_create(const OmDataPath & path, int size, const OmDataWrapperPtr data)
{
	HDF5_CHECK();
	hdfLowLevel.dataset_raw_create(fileId, name, size, data);
}

void OmHdf5Impl::dataset_raw_create_tree_overwrite(const OmDataPath & path, int size, const OmDataWrapperPtr data)
{
	HDF5_CHECK();

	//create tree and delete old data if exists
	hdfLowLevel.dataset_delete_create_tree(fileId, name);

	//create data
	hdfLowLevel.dataset_raw_create(fileId, name, size, data);
}

//imageIo
Vector3 < int > OmHdf5Impl::dataset_image_get_dims(const OmDataPath & path)
{
	HDF5_CHECK();
	return hdfLowLevel.dataset_image_get_dims(fileId, name);
}

OmDataWrapperPtr OmHdf5Impl::dataset_read_raw_chunk_data(const OmDataPath & path, DataBbox dataExtent)
{
	HDF5_CHECK();
	return hdfLowLevel.dataset_read_raw_chunk_data(fileId, name, dataExtent);
}

void OmHdf5Impl::dataset_write_raw_chunk_data(const OmDataPath & path, DataBbox dataExtent, OmDataWrapperPtr data)
{
	HDF5_CHECK();
	hdfLowLevel.dataset_write_raw_chunk_data(fileId, name, dataExtent, data);
}

Vector3< int > OmHdf5Impl::dataset_get_dims( const OmDataPath & path )
{
	HDF5_CHECK();
	return hdfLowLevel.dataset_get_dims(fileId, name);
}

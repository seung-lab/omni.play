#include "common/omDebug.h"
#include "common/omException.h"
#include "datalayer/hdf5/omHdf5Impl.h"
#include "datalayer/omDataPath.h"
#include "datalayer/hdf5/omHdf5Utils.hpp"

OmHdf5Impl::OmHdf5Impl(string fileName, const bool readOnly)
	: mReadOnly(readOnly)
{
	fileId = OmHdf5FileUtils::file_open(fileName, mReadOnly);
	OmHdf5LowLevel* hdf = new OmHdf5LowLevel(fileId);
	hdf_ = boost::shared_ptr<OmHdf5LowLevel>(hdf);
}

OmHdf5Impl::~OmHdf5Impl()
{
	OmHdf5FileUtils::file_close(fileId);
}

void OmHdf5Impl::flush()
{
	OmHdf5FileUtils::flush(fileId);
}

bool OmHdf5Impl::group_exists(const OmDataPath & path)
{
	hdf_->setPath(path);
	return hdf_->group_exists();
}

void OmHdf5Impl::group_delete(const OmDataPath & path)
{
	hdf_->setPath(path);
	hdf_->group_delete();
}

bool OmHdf5Impl::dataset_exists(const OmDataPath & path)
{
	hdf_->setPath(path);
	return hdf_->dataset_exists();
}

void OmHdf5Impl::
dataset_image_create_tree_overwrite(const OmDataPath & path,
				    const Vector3i& dataDims,
				    const Vector3i& chunkDims,
				    const OmVolDataType type)
{
	hdf_->setPath(path);
	hdf_->dataset_delete_create_tree();
	hdf_->dataset_image_create(dataDims, chunkDims, type);
}

OmDataWrapperPtr OmHdf5Impl::dataset_image_read_trim(const OmDataPath & path, DataBbox dataExtent)
{
	hdf_->setPath(path);
	return hdf_->dataset_image_read_trim(dataExtent);
}

OmDataWrapperPtr OmHdf5Impl::dataset_raw_read(const OmDataPath & path, int *size)
{
	hdf_->setPath(path);
	return hdf_->dataset_raw_read(size);
}

void OmHdf5Impl::dataset_raw_create(const OmDataPath & path, int size, const OmDataWrapperPtr data)
{
	hdf_->setPath(path);
	hdf_->dataset_raw_create(size, data);
}

void OmHdf5Impl::dataset_raw_create_tree_overwrite(const OmDataPath & path, int size, const OmDataWrapperPtr data)
{
	hdf_->setPath(path);

	//create tree and delete old data if exists
	hdf_->dataset_delete_create_tree();

	//create data
	hdf_->dataset_raw_create(size, data);
}

Vector3i OmHdf5Impl::dataset_image_get_dims(const OmDataPath & path)
{
	hdf_->setPath(path);
	return hdf_->dataset_image_get_dims();
}

OmDataWrapperPtr OmHdf5Impl::dataset_read_raw_chunk_data(const OmDataPath & path, DataBbox dataExtent)
{
	hdf_->setPath(path);
	return hdf_->dataset_read_raw_chunk_data(dataExtent);
}

void OmHdf5Impl::dataset_write_raw_chunk_data(const OmDataPath & path, DataBbox dataExtent, OmDataWrapperPtr data)
{
	hdf_->setPath(path);
	hdf_->dataset_write_raw_chunk_data(dataExtent, data);
}

Vector3< int > OmHdf5Impl::dataset_get_dims( const OmDataPath & path )
{
	hdf_->setPath(path);
	return hdf_->dataset_get_dims();
}

#include "common/omDebug.h"
#include "common/omException.h"
#include "datalayer/hdf5/omHdf5Impl.h"
#include "datalayer/omDataPath.h"
#include "datalayer/hdf5/omHdf5FileUtils.hpp"

OmHdf5Impl::OmHdf5Impl(const std::string& fileName, const bool readOnly)
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

void OmHdf5Impl::allocateChunkedDataset(const OmDataPath & path,
					const Vector3i& dataDims,
					const Vector3i& chunkDims,
					const OmVolDataType type)
{
	hdf_->setPath(path);
	hdf_->dataset_delete_create_tree();
	hdf_->allocateChunkedDataset(dataDims, chunkDims, type);
}

OmDataWrapperPtr OmHdf5Impl::readDataset(const OmDataPath & path,
					      int *size)
{
	hdf_->setPath(path);
	return hdf_->readDataset(size);
}

void OmHdf5Impl::writeDataset(const OmDataPath & path,
			      int size,
			      const OmDataWrapperPtr data)
{
	hdf_->setPath(path);

	//create tree and delete old data if exists
	hdf_->dataset_delete_create_tree();

	//create data
	hdf_->allocateDataset(size, data);
}

Vector3i OmHdf5Impl::getChunkedDatasetDims(const OmDataPath & path)
{
	hdf_->setPath(path);
	return hdf_->getChunkedDatasetDims();
}

OmDataWrapperPtr OmHdf5Impl::readChunk(const OmDataPath & path,
				       DataBbox dataExtent)
{
	hdf_->setPath(path);
	return hdf_->readChunk(dataExtent);
}

void OmHdf5Impl::writeChunk(const OmDataPath & path,
			    DataBbox dataExtent,
			    OmDataWrapperPtr data)
{
	hdf_->setPath(path);
	hdf_->writeChunk(dataExtent, data);
}

Vector3< int > OmHdf5Impl::getDatasetDims( const OmDataPath & path )
{
	hdf_->setPath(path);
	return hdf_->getDatasetDims();
}

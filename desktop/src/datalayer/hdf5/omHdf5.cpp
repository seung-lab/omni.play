#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/hdf5/omHdf5Impl.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/hdf5/omHdf5FileUtils.hpp"

void OmHdf5::create()
{
    zi::rwmutex::write_guard g(fileLock);
    OmHdf5FileUtils::file_create(fnp_);
}

void OmHdf5::open()
{
    zi::rwmutex::write_guard g(fileLock);

    hdf5_ = std::make_shared<OmHdf5Impl>(fnp_, readOnly_);

    std::cout << "opened hdf5 file " << fnp_ << "\n";
}

void OmHdf5::close()
{
    zi::rwmutex::write_guard g(fileLock);
    hdf5_.reset();
}

void OmHdf5::flush()
{
    zi::rwmutex::write_guard g(fileLock);
    hdf5_->flush();
}

bool OmHdf5::group_exists(const OmDataPath& path)
{
    zi::rwmutex::write_guard g(fileLock);
    return hdf5_->group_exists(path);
}

void OmHdf5::group_delete(const OmDataPath& path)
{
    zi::rwmutex::write_guard g(fileLock);
    hdf5_->group_delete(path);
}

bool OmHdf5::dataset_exists(const OmDataPath& path)
{
    zi::rwmutex::write_guard g(fileLock);
    return hdf5_->dataset_exists(path);
}

void OmHdf5::allocateChunkedDataset(const OmDataPath& path,
                                    const Vector3i& dataDims,
                                    const Vector3i& chunkDims,
                                    const OmVolDataType type)
{
    zi::rwmutex::write_guard g(fileLock);
    hdf5_->allocateChunkedDataset(path, dataDims, chunkDims, type);
}

OmDataWrapperPtr OmHdf5::readDataset(const OmDataPath& path, int* size)
{
    zi::rwmutex::write_guard g(fileLock);
    return hdf5_->readDataset(path, size);
}

OmDataWrapperPtr OmHdf5::readChunk(const OmDataPath& path,
                                   const om::dataBbox& dataExtent,
                                   const om::common::AffinityGraph aff)
{
    zi::rwmutex::write_guard g(fileLock);
    return hdf5_->readChunk(path, dataExtent, aff);
}

void OmHdf5::writeChunk(const OmDataPath& path,
                        om::dataBbox dataExtent,
                        OmDataWrapperPtr data)
{
    zi::rwmutex::write_guard g(fileLock);
    hdf5_->writeChunk(path, dataExtent, data);
}

OmDataWrapperPtr OmHdf5::GetChunkDataType(const OmDataPath& path)
{
    zi::rwmutex::write_guard g(fileLock);
    return hdf5_->GetChunkDataType(path);
}

void OmHdf5::writeDataset(const OmDataPath& path,
                          int size,
                          const OmDataWrapperPtr data)
{
    if (!size){
        return;
    }

    zi::rwmutex::write_guard g(fileLock);
    hdf5_->writeDataset(path, size, data);
}

Vector3i OmHdf5::getChunkedDatasetDims(const OmDataPath& path,
                                       const om::common::AffinityGraph aff)
{
    zi::rwmutex::write_guard g(fileLock);
    return hdf5_->getChunkedDatasetDims(path, aff);
}

Vector3i OmHdf5::getDatasetDims(const OmDataPath& path)
{
    zi::rwmutex::write_guard g(fileLock);
    return hdf5_->getDatasetDims(path);
}

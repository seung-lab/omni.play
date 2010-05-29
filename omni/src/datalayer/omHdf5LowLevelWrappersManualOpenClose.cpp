#include "common/omDebug.h"
#include "common/omException.h"
#include "datalayer/omHdf5LowLevelWrappersManualOpenClose.h"
#include "datalayer/omHdf5Path.h"

#define HDF5_CHECK() 						\
	string pathStr = path.getString();                      \
	const char * name = pathStr.c_str();                    \
	if(!opened) {                                           \
                throw OmIoException("HDF5 file was not open");  \
	}

OmHdf5LowLevelWrappersManualOpenClose::OmHdf5LowLevelWrappersManualOpenClose(string fileName, const bool readOnly)
	: mFileName(fileName)
	, mReadOnly(readOnly)
	, fileId(-1)
	, opened(false)
{
}

OmHdf5LowLevelWrappersManualOpenClose::~OmHdf5LowLevelWrappersManualOpenClose()
{
	close();
}

void OmHdf5LowLevelWrappersManualOpenClose::open()
{
	if(opened) {
		assert(0);
                throw OmIoException("HDF5 file was already open");
	}

	fileId = hdfLowLevel.om_hdf5_file_open_with_lock(mFileName, mReadOnly);
	opened = true;
}

void OmHdf5LowLevelWrappersManualOpenClose::close()
{
	if(!opened) {
		return;
	}

	hdfLowLevel.om_hdf5_file_close_with_lock(fileId);
	opened = false;
}

/////////////////////////////////
///////          File
void OmHdf5LowLevelWrappersManualOpenClose::file_create()
{
	hdfLowLevel.om_hdf5_file_create( mFileName );
}

void OmHdf5LowLevelWrappersManualOpenClose::flush()
{
	hdfLowLevel.om_hdf5_flush_with_lock(fileId);
}

/////////////////////////////////
///////          Group
bool OmHdf5LowLevelWrappersManualOpenClose::group_exists_with_lock(const OmHdf5Path & path)
{
	HDF5_CHECK();
	return hdfLowLevel.om_hdf5_group_exists_with_lock(fileId, name);
}

void OmHdf5LowLevelWrappersManualOpenClose::group_delete_with_lock(const OmHdf5Path & path)
{
	HDF5_CHECK();
	hdfLowLevel.om_hdf5_group_delete_with_lock(fileId, name);
}

/////////////////////////////////
///////          Dataset

bool OmHdf5LowLevelWrappersManualOpenClose::dataset_exists_with_lock(const OmHdf5Path & path)
{
	HDF5_CHECK();
	return hdfLowLevel.om_hdf5_dataset_exists_with_lock(fileId, name);
}

void OmHdf5LowLevelWrappersManualOpenClose::dataset_image_create_tree_overwrite_with_lock(const OmHdf5Path & path, Vector3<int>* dataDims,
					    Vector3<int>* chunkDims, int bytesPerSample)
{
	HDF5_CHECK();
	hdfLowLevel.om_hdf5_dataset_delete_create_tree_with_lock(fileId, name);
	hdfLowLevel.om_hdf5_dataset_image_create_with_lock(fileId, name, dataDims, chunkDims, bytesPerSample);
}

vtkImageData * OmHdf5LowLevelWrappersManualOpenClose::dataset_image_read_trim_with_lock(const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample)
{
	HDF5_CHECK();
	return hdfLowLevel.om_hdf5_dataset_image_read_trim_with_lock(fileId, name, dataExtent, bytesPerSample);
}

void OmHdf5LowLevelWrappersManualOpenClose::dataset_image_write_trim_with_lock(const OmHdf5Path & path, DataBbox* dataExtent, int bytesPerSample,
				 vtkImageData * pImageData)
{
	HDF5_CHECK();
	hdfLowLevel.om_hdf5_dataset_image_write_trim_with_lock(fileId, name, dataExtent, bytesPerSample, pImageData);
}

void * OmHdf5LowLevelWrappersManualOpenClose::dataset_raw_read_with_lock(const OmHdf5Path & path, int *size)
{
	HDF5_CHECK();
	return hdfLowLevel.om_hdf5_dataset_raw_read_with_lock(fileId, name, size);
}

void OmHdf5LowLevelWrappersManualOpenClose::dataset_raw_create_with_lock(const OmHdf5Path & path, int size, const void *data)
{
	HDF5_CHECK();
	hdfLowLevel.om_hdf5_dataset_raw_create_with_lock(fileId, name, size, data);
}

void OmHdf5LowLevelWrappersManualOpenClose::dataset_raw_create_tree_overwrite_with_lock(const OmHdf5Path & path, int size, const void* data)
{
	HDF5_CHECK();
	hdfLowLevel.om_hdf5_dataset_raw_create_tree_overwrite_with_lock(fileId, name, size, data);
}

//imageIo
Vector3 < int > OmHdf5LowLevelWrappersManualOpenClose::dataset_image_get_dims_with_lock(const OmHdf5Path & path)
{
	HDF5_CHECK();
	return hdfLowLevel.om_hdf5_dataset_image_get_dims_with_lock(fileId, name);
}

void* OmHdf5LowLevelWrappersManualOpenClose::dataset_read_raw_chunk_data(const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample)
{
	HDF5_CHECK();
	return hdfLowLevel.om_hdf5_dataset_read_raw_chunk_data(fileId, name, dataExtent, bytesPerSample);
}

void OmHdf5LowLevelWrappersManualOpenClose::dataset_write_raw_chunk_data(const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample, void * imageData)
{
	HDF5_CHECK();
	hdfLowLevel.om_hdf5_dataset_write_raw_chunk_data(fileId, name, dataExtent, bytesPerSample,  imageData);
}

Vector3< int > OmHdf5LowLevelWrappersManualOpenClose::dataset_get_dims_with_lock( const OmHdf5Path & path )
{
	HDF5_CHECK();
	return hdfLowLevel.om_hdf5_dataset_get_dims_with_lock(fileId, name);
}

#include "common/omDebug.h"
#include "common/omException.h"
#include "datalayer/omHdf5LowLevelWrappersManualOpenClose.h"
#include "datalayer/omHdf5Path.h"

#define HDF5_WRAP() 								\
	string pathStr = path.getString();                                      \
	const char * name = pathStr.c_str();                                    \
	if(!opened) {                                                           \
                throw OmIoException("HDF5 file was not open");                  \
	}
#define HDF5_UNWRAP()

OmHdf5LowLevelWrappersManualOpenClose::OmHdf5LowLevelWrappersManualOpenClose(string fileName, const bool readOnly)
	: mFileName(fileName), mReadOnly(readOnly), fileId(-1), opened(false)
{
}

OmHdf5LowLevelWrappersManualOpenClose::~OmHdf5LowLevelWrappersManualOpenClose()
{
	close();
}

void OmHdf5LowLevelWrappersManualOpenClose::open()
{
	if(opened) {
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
	bool exists;
	HDF5_WRAP();
	exists = hdfLowLevel.om_hdf5_group_exists_with_lock(fileId, name);
	HDF5_UNWRAP();
	return exists;
}

void OmHdf5LowLevelWrappersManualOpenClose::group_delete_with_lock(const OmHdf5Path & path)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_group_delete_with_lock(fileId, name);
	HDF5_UNWRAP();
}

/////////////////////////////////
///////          Dataset

bool OmHdf5LowLevelWrappersManualOpenClose::dataset_exists_with_lock(const OmHdf5Path & path)
{
	bool exists;
	HDF5_WRAP();
	exists = hdfLowLevel.om_hdf5_dataset_exists_with_lock(fileId, name);
	HDF5_UNWRAP();
	return exists;
}

void OmHdf5LowLevelWrappersManualOpenClose::dataset_image_create_tree_overwrite_with_lock(const OmHdf5Path & path, Vector3<int>* dataDims,
					    Vector3<int>* chunkDims, int bytesPerSample)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_delete_create_tree_with_lock(fileId, name);
	hdfLowLevel.om_hdf5_dataset_image_create_with_lock(fileId, name, dataDims, chunkDims, bytesPerSample);
	HDF5_UNWRAP();
}

vtkImageData * OmHdf5LowLevelWrappersManualOpenClose::dataset_image_read_trim_with_lock(const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample)
{
	vtkImageData * ret;
	HDF5_WRAP();
	ret = hdfLowLevel.om_hdf5_dataset_image_read_trim_with_lock(fileId, name, dataExtent, bytesPerSample);
	HDF5_UNWRAP();
	return ret;
}

void OmHdf5LowLevelWrappersManualOpenClose::dataset_image_write_trim_with_lock(const OmHdf5Path & path, DataBbox* dataExtent, int bytesPerSample,
				 vtkImageData * pImageData)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_image_write_trim_with_lock(fileId, name, dataExtent, bytesPerSample, pImageData);
	HDF5_UNWRAP();
}

void * OmHdf5LowLevelWrappersManualOpenClose::dataset_raw_read_with_lock(const OmHdf5Path & path, int *size)
{
	void * ret;
	HDF5_WRAP();
	ret = hdfLowLevel.om_hdf5_dataset_raw_read_with_lock(fileId, name, size);
	HDF5_UNWRAP();
	return ret;
}

void OmHdf5LowLevelWrappersManualOpenClose::dataset_raw_create_with_lock(const OmHdf5Path & path, int size, const void *data)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_raw_create_with_lock(fileId, name, size, data);
	HDF5_UNWRAP();
}

void OmHdf5LowLevelWrappersManualOpenClose::dataset_raw_create_tree_overwrite_with_lock(const OmHdf5Path & path, int size, const void* data)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_raw_create_tree_overwrite_with_lock(fileId, name, size, data);
	HDF5_UNWRAP();
}

//imageIo
Vector3 < int > OmHdf5LowLevelWrappersManualOpenClose::dataset_image_get_dims_with_lock(const OmHdf5Path & path)
{
	Vector3<int> ret;
	HDF5_WRAP();
	ret = hdfLowLevel.om_hdf5_dataset_image_get_dims_with_lock(fileId, name);
	HDF5_UNWRAP();

	return ret;
}

void* OmHdf5LowLevelWrappersManualOpenClose::dataset_read_raw_chunk_data(const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample)
{
	void* ret;
	HDF5_WRAP();
	ret = hdfLowLevel.om_hdf5_dataset_read_raw_chunk_data(fileId, name, dataExtent, bytesPerSample);
	HDF5_UNWRAP();
	return ret;
}

void OmHdf5LowLevelWrappersManualOpenClose::dataset_write_raw_chunk_data(const OmHdf5Path & path, DataBbox dataExtent, int bytesPerSample, void * imageData)
{
	HDF5_WRAP();
	hdfLowLevel.om_hdf5_dataset_write_raw_chunk_data(fileId, name, dataExtent, bytesPerSample,  imageData);
	HDF5_UNWRAP();
}

Vector3< int > OmHdf5LowLevelWrappersManualOpenClose::dataset_get_dims_with_lock( const OmHdf5Path & path )
{
	Vector3<int> ret;
	HDF5_WRAP();
	ret = hdfLowLevel.om_hdf5_dataset_get_dims_with_lock(fileId, name);
	HDF5_UNWRAP();

	return ret;	
}
